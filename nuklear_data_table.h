#ifndef NK_DATA_TABLE_H_
#define NK_DATA_TABLE_H_

struct nk_data_table {
    int decl_col_count;
    int col_count;

    int row_col_count;

    struct nk_context *ctx;
};

nk_bool nk_data_table_begin(struct nk_context *ctx, struct nk_data_table *table, int col_count);
void nk_data_table_end(struct nk_data_table *table);
void nk_data_table_column(struct nk_data_table *table, const char *label);
void nk_data_table_next_row(struct nk_data_table *table);
void nk_data_table_cell(struct nk_data_table *table, const char *str);

#ifdef NK_INCLUDE_STANDARD_VARARGS
void nk_data_table_cellf(struct nk_data_table *table, const char *fmt, ...);
#endif /* NK_INCLUDE_STANDARD_VARARGS */

#endif /* NK_DATA_TABLE_H_ */


#ifdef NK_DATA_TABLE_IMPLEMENTATION

nk_bool nk_data_table_begin(struct nk_context *ctx, struct nk_data_table *table, int col_count) {
    NK_ASSERT(ctx);
    NK_ASSERT(table);
    NK_ASSERT(col_count > 0);
    if (!ctx || !table || col_count < 1) return 0;

    table->decl_col_count = col_count;
    table->col_count = 0;
    table->row_col_count = 0;
    table->ctx = ctx;

    nk_layout_row_dynamic(ctx, 0, table->decl_col_count);

    return 1;
}

void nk_data_table_end(struct nk_data_table *table) {
    NK_UNUSED(table);
    return;
}

void nk_data_table_column(struct nk_data_table *table, const char *label) {
    NK_ASSERT(table);
    NK_ASSERT(label);
    NK_ASSERT(table->col_count < table->decl_col_count);
    if (!table || !label || table->col_count >= table->decl_col_count) return;

    nk_label(table->ctx, label, NK_TEXT_LEFT);
    table->col_count++;
}

void nk_data_table_next_row(struct nk_data_table *table) {
    table->row_col_count = 0;
}

void nk_data_table_cell(struct nk_data_table *table, const char *str) {
    NK_ASSERT(table);
    NK_ASSERT(str);
    NK_ASSERT(table->row_col_count < table->decl_col_count);
    if (!table || !str || table->row_col_count >= table->decl_col_count) return;

    nk_label(table->ctx, str, NK_TEXT_LEFT);
    table->row_col_count++;
}

#ifdef NK_INCLUDE_STANDARD_VARARGS
void nk_data_table_cellf(struct nk_data_table *table, const char *fmt, ...) {
    va_list args;

    NK_ASSERT(table);
    NK_ASSERT(table->row_col_count < table->decl_col_count);
    if (!table || table->row_col_count >= table->decl_col_count) return;

    va_start(args, fmt);
    nk_labelfv(table->ctx, NK_TEXT_LEFT, fmt, args);
    va_end(args);

    table->row_col_count++;
}
#endif /* NK_INCLUDE_STANDARD_VARARGS */

#endif /* NK_DATA_TABLE_IMPLEMENTATION */
