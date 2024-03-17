#include <archive.h>
#include <archive_entry.h>

#include "builtins.h"

static ObjModule *_archive_tar_module = NULL;

static BuiltinResult _compress_tar_list_entries(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct archive_entry *entry;
    struct archive *a = archive_read_new();

    archive_read_support_filter_gzip(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_xz(a);

    archive_read_support_format_tar(a);
    archive_read_support_format_gnutar(a);

    ObjList *entries = new_list(NULL, 0);

    if (archive_read_open_filename(a, argv_0->chars, 10240) != ARCHIVE_OK)
        return ERR("Could not open archive.");

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *entry_name = archive_entry_pathname(entry);
        write_array(&entries->elems, AS_OBJ(new_string(entry_name, strlen(entry_name))));
        archive_read_data_skip(a);
    }

    archive_read_close(a);
    if (archive_read_free(a) != ARCHIVE_OK)
        return ERR("Could not close archive.");

    return OK(entries);
}

static BuiltinResult _compress_tar_extract(int argc, Obj **argv, UNUSED(Obj *, caller)) {
    CHECK_ARG_COUNT(1)
    CHECK_ARG_TYPE(ObjString, STRING, 0)

    struct archive_entry *entry;
    struct archive *a = archive_read_new();

    archive_read_support_filter_gzip(a);
    archive_read_support_filter_bzip2(a);
    archive_read_support_filter_xz(a);

    archive_read_support_format_tar(a);
    archive_read_support_format_gnutar(a);

    struct archive *ext = archive_write_disk_new();
    archive_write_disk_set_options(
        ext,
        ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);
    archive_write_disk_set_standard_lookup(ext);

    if (archive_read_open_filename(a, argv_0->chars, 10240) != ARCHIVE_OK)
        return ERR("Could not open archive.");

    int r;
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            return ERR(archive_error_string(a));
        if (r < ARCHIVE_WARN)
            return ERR("Could not read header.");

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            return ERR(archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            const void *buff;
            size_t size;
            la_int64_t offset;

            while ((r = archive_read_data_block(a, &buff, &size, &offset)) == ARCHIVE_OK) {

                if (r == ARCHIVE_EOF)
                    break;
                r = archive_write_data_block(ext, buff, size, offset);

                if (r < ARCHIVE_OK) {
                    return ERR(archive_error_string(ext));
                }
            }

            if (r < ARCHIVE_OK)
                return ERR(archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                return ERR("Could not write header.");
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            return ERR(archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            return ERR("Could not finish writing entry.");
    }

    archive_read_close(a);
    archive_read_free(a);

    archive_write_close(ext);
    archive_write_free(ext);

    return OK(new_nil());
}

ObjModule *get_archive_tar_module() {
    if (_archive_tar_module == NULL) {
        ObjModule *module = new_module(new_string("tar", 3));

        SET_BUILTIN_FN_MEMBER("list_entries", _compress_tar_list_entries);
        SET_BUILTIN_FN_MEMBER("extract", _compress_tar_extract);

        _archive_tar_module = module;
    }

    return _archive_tar_module;
}
