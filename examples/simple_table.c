#include <SDL3/SDL.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#define NK_INCLUDE_COMMAND_USERDATA
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT

#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_STANDARD_IO

#define NK_IMPLEMENTATION
#include "thirdparty/nuklear.h"
#define NK_SDL3_RENDERER_IMPLEMENTATION
#include "thirdparty/nuklear_sdl3_renderer.h"


struct table_row {
    int id;
    const char *name;
    int qty;
    const char *desc;
};

static struct table_row table_rows[] = {
    {1, "Banana",  0, "Lorem ipsum dolor sit amet"},
    {2, "Apple",  10, "Lorem ipsum dolor sit amet"},
    {3, "Cherry", 20, "Lorem ipsum dolor sit amet"}
};

static void simple_table(struct nk_context *ctx) {
    nk_flags flags = NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE;
    if (nk_begin(ctx, "Simple Table", nk_rect(50, 50, 800, 600), flags)) {
        size_t i = 0;
        nk_layout_row_dynamic(ctx, 0, 4);
        /* Table headers */
        nk_label(ctx, "ID", NK_TEXT_LEFT);
        nk_label(ctx, "Name", NK_TEXT_LEFT);
        nk_label(ctx, "Quantity", NK_TEXT_LEFT);
        nk_label(ctx, "Description", NK_TEXT_LEFT);

        /* Table rows */
        for (i = 0; i < NK_LEN(table_rows); i++) {
            struct table_row *row = &table_rows[i];
            nk_labelf(ctx, NK_TEXT_LEFT, "%d", row->id);
            nk_label(ctx, row->name, NK_TEXT_LEFT);
            nk_labelf(ctx, NK_TEXT_LEFT, "%d", row->qty);
            nk_label(ctx, row->desc, NK_TEXT_LEFT);
        }
    }
    nk_end(ctx);
}


#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

struct nk_sdl_app {
    SDL_Window* window;
    SDL_Renderer* renderer;
    struct nk_context * ctx;
    struct nk_colorf bg;
    enum nk_anti_aliasing AA;
};

static SDL_AppResult
nk_sdl_fail()
{
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error: %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[])
{
    struct nk_sdl_app* app;
    struct nk_context* ctx;
    float font_scale;
    NK_UNUSED(argc);
    NK_UNUSED(argv);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        return nk_sdl_fail();
    }

    app = SDL_malloc(sizeof(*app));
    if (app == NULL) {
        return nk_sdl_fail();
    }

    if (!SDL_CreateWindowAndRenderer("Nuklear Datatable", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &app->window, &app->renderer)) {
        SDL_free(app);
        return nk_sdl_fail();
    }
    *appstate = app;

    if (!SDL_SetRenderVSync(app->renderer, 1)) {
        SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "SDL_SetRenderVSync failed: %s", SDL_GetError());
    }

    app->bg.r = 0.10f;
    app->bg.g = 0.18f;
    app->bg.b = 0.24f;
    app->bg.a = 1.0f;


    font_scale = 1;
    {
        const float scale = SDL_GetWindowDisplayScale(app->window);
        SDL_SetRenderScale(app->renderer, scale, scale);
        font_scale = scale;
    }

    ctx = nk_sdl_init(app->window, app->renderer, nk_sdl_allocator());
    app->ctx = ctx;

    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        atlas = nk_sdl_font_stash_begin(ctx);
        font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
        nk_sdl_font_stash_end(ctx);

        font->handle.height /= font_scale;
        nk_style_set_font(ctx, &font->handle);

        app->AA = NK_ANTI_ALIASING_ON;
    }

    nk_input_begin(ctx);

    return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event* event)
{
    struct nk_sdl_app* app = (struct nk_sdl_app*)appstate;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_Q && event->key.mod & SDL_KMOD_CTRL) {
                return SDL_APP_SUCCESS;
            }
            break;
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            SDL_Log("Unhandled scale event! Nuklear may appear blurry");
            return SDL_APP_CONTINUE;
    }

    SDL_ConvertEventToRenderCoordinates(app->renderer, event);

    nk_sdl_handle_event(app->ctx, event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
    struct nk_sdl_app* app = (struct nk_sdl_app*)appstate;
    struct nk_context* ctx = app->ctx;

    nk_input_end(ctx);

    simple_table(ctx);

    SDL_SetRenderDrawColorFloat(app->renderer, app->bg.r, app->bg.g, app->bg.b, app->bg.a);
    SDL_RenderClear(app->renderer);

    nk_sdl_render(ctx, app->AA);
    nk_sdl_update_TextInput(ctx);

    SDL_RenderPresent(app->renderer);

    nk_input_begin(ctx);
    return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    struct nk_sdl_app* app = (struct nk_sdl_app*)appstate;
    NK_UNUSED(result);

    if (app) {
        nk_input_end(app->ctx);
        nk_sdl_shutdown(app->ctx);
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        SDL_free(app);
    }
}
