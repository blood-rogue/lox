#include <archive.h>
#include <archive_entry.h>

#include "native.h"

static ObjModule *_archive_module = NULL;

static void free_archive(void *archive) { archive_read_free(archive); }

static NativeResult _archive_open(int argc, Obj **argv, UNUSED(Obj *caller)) {
    CHECK_ARG_COUNT(3)
    CHECK_ARG_TYPE(ObjString, STRING, 0)
    CHECK_ARG_TYPE(ObjInt, INT, 1)
    CHECK_ARG_TYPE(ObjInt, INT, 2)

    struct archive *a = archive_read_new();

    archive_read_support_format_by_code(a, mpz_get_si(argv_1->value));
    archive_read_support_filter_by_code(a, mpz_get_si(argv_2->value));

    ObjNativeStruct *archive_obj = new_native_struct(a, free_archive);

    if (archive_read_open_filename(a, argv_0->chars, 10240) != ARCHIVE_OK)
        ERR("Could not open archive.")

    ObjInstance *instance = new_instance(get_archive_file_class());

    SET_FIELD("$$internal", archive_obj);

    OK(instance);
}

ObjModule *get_archive_module(int count, UNUSED(char **parts)) {
    CHECK_PART_COUNT

    if (_archive_module == NULL) {
        ObjModule *module = new_module("archive");

        SET_NATIVE_FN_MEMBER("open", _archive_open);

        SET_INT_MEMBER("SYMLINK_TYPE_DIRECTORY", AE_SYMLINK_TYPE_DIRECTORY);
        SET_INT_MEMBER("SYMLINK_TYPE_FILE", AE_SYMLINK_TYPE_FILE);
        SET_INT_MEMBER("SYMLINK_TYPE_UNDEFINED", AE_SYMLINK_TYPE_UNDEFINED);

        SET_INT_MEMBER("FILTER_NONE", ARCHIVE_FILTER_NONE);
        SET_INT_MEMBER("FILTER_GZIP", ARCHIVE_FILTER_GZIP);
        SET_INT_MEMBER("FILTER_BZIP2", ARCHIVE_FILTER_BZIP2);
        SET_INT_MEMBER("FILTER_COMPRESS", ARCHIVE_FILTER_COMPRESS);
        SET_INT_MEMBER("FILTER_PROGRAM", ARCHIVE_FILTER_PROGRAM);
        SET_INT_MEMBER("FILTER_LZMA", ARCHIVE_FILTER_LZMA);
        SET_INT_MEMBER("FILTER_XZ", ARCHIVE_FILTER_XZ);
        SET_INT_MEMBER("FILTER_UU", ARCHIVE_FILTER_UU);
        SET_INT_MEMBER("FILTER_RPM", ARCHIVE_FILTER_RPM);
        SET_INT_MEMBER("FILTER_LZIP", ARCHIVE_FILTER_LZIP);
        SET_INT_MEMBER("FILTER_LRZIP", ARCHIVE_FILTER_LRZIP);
        SET_INT_MEMBER("FILTER_LZOP", ARCHIVE_FILTER_LZOP);
        SET_INT_MEMBER("FILTER_GRZIP", ARCHIVE_FILTER_GRZIP);
        SET_INT_MEMBER("FILTER_LZ4", ARCHIVE_FILTER_LZ4);
        SET_INT_MEMBER("FILTER_ZSTD", ARCHIVE_FILTER_ZSTD);

        SET_INT_MEMBER("FORMAT_CPIO", ARCHIVE_FORMAT_CPIO);
        SET_INT_MEMBER("FORMAT_CPIO_POSIX", ARCHIVE_FORMAT_CPIO_POSIX);
        SET_INT_MEMBER("FORMAT_CPIO_BIN_LE", ARCHIVE_FORMAT_CPIO_BIN_LE);
        SET_INT_MEMBER("FORMAT_CPIO_BIN_BE", ARCHIVE_FORMAT_CPIO_BIN_BE);
        SET_INT_MEMBER("FORMAT_CPIO_SVR4_NOCRC", ARCHIVE_FORMAT_CPIO_SVR4_NOCRC);
        SET_INT_MEMBER("FORMAT_CPIO_SVR4_CRC", ARCHIVE_FORMAT_CPIO_SVR4_CRC);
        SET_INT_MEMBER("FORMAT_CPIO_AFIO_LARGE", ARCHIVE_FORMAT_CPIO_AFIO_LARGE);
        SET_INT_MEMBER("FORMAT_CPIO_PWB", ARCHIVE_FORMAT_CPIO_PWB);
        SET_INT_MEMBER("FORMAT_SHAR", ARCHIVE_FORMAT_SHAR);
        SET_INT_MEMBER("FORMAT_SHAR_BASE", ARCHIVE_FORMAT_SHAR_BASE);
        SET_INT_MEMBER("FORMAT_SHAR_DUMP", ARCHIVE_FORMAT_SHAR_DUMP);
        SET_INT_MEMBER("FORMAT_TAR", ARCHIVE_FORMAT_TAR);
        SET_INT_MEMBER("FORMAT_TAR_USTAR", ARCHIVE_FORMAT_TAR_USTAR);
        SET_INT_MEMBER("FORMAT_TAR_PAX_INTERCHANGE", ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE);
        SET_INT_MEMBER("FORMAT_TAR_PAX_RESTRICTED", ARCHIVE_FORMAT_TAR_PAX_RESTRICTED);
        SET_INT_MEMBER("FORMAT_TAR_GNUTAR", ARCHIVE_FORMAT_TAR_GNUTAR);
        SET_INT_MEMBER("FORMAT_ISO9660", ARCHIVE_FORMAT_ISO9660);
        SET_INT_MEMBER("FORMAT_ISO9660_ROCKRIDGE", ARCHIVE_FORMAT_ISO9660_ROCKRIDGE);
        SET_INT_MEMBER("FORMAT_ZIP", ARCHIVE_FORMAT_ZIP);
        SET_INT_MEMBER("FORMAT_EMPTY", ARCHIVE_FORMAT_EMPTY);
        SET_INT_MEMBER("FORMAT_AR", ARCHIVE_FORMAT_AR);
        SET_INT_MEMBER("FORMAT_AR_GNU", ARCHIVE_FORMAT_AR_GNU);
        SET_INT_MEMBER("FORMAT_AR_BSD", ARCHIVE_FORMAT_AR_BSD);
        SET_INT_MEMBER("FORMAT_MTREE", ARCHIVE_FORMAT_MTREE);
        SET_INT_MEMBER("FORMAT_RAW", ARCHIVE_FORMAT_RAW);
        SET_INT_MEMBER("FORMAT_XAR", ARCHIVE_FORMAT_XAR);
        SET_INT_MEMBER("FORMAT_LHA", ARCHIVE_FORMAT_LHA);
        SET_INT_MEMBER("FORMAT_CAB", ARCHIVE_FORMAT_CAB);
        SET_INT_MEMBER("FORMAT_RAR", ARCHIVE_FORMAT_RAR);
        SET_INT_MEMBER("FORMAT_7ZIP", ARCHIVE_FORMAT_7ZIP);
        SET_INT_MEMBER("FORMAT_WARC", ARCHIVE_FORMAT_WARC);
        SET_INT_MEMBER("FORMAT_RAR_V5", ARCHIVE_FORMAT_RAR_V5);

        SET_INT_MEMBER("EXTRACT_OWNER", ARCHIVE_EXTRACT_OWNER);
        SET_INT_MEMBER("EXTRACT_PERM", ARCHIVE_EXTRACT_PERM);
        SET_INT_MEMBER("EXTRACT_TIME", ARCHIVE_EXTRACT_TIME);
        SET_INT_MEMBER("EXTRACT_NO_OVERWRITE", ARCHIVE_EXTRACT_NO_OVERWRITE);
        SET_INT_MEMBER("EXTRACT_UNLINK", ARCHIVE_EXTRACT_UNLINK);
        SET_INT_MEMBER("EXTRACT_ACL", ARCHIVE_EXTRACT_ACL);
        SET_INT_MEMBER("EXTRACT_FFLAGS", ARCHIVE_EXTRACT_FFLAGS);
        SET_INT_MEMBER("EXTRACT_XATTR", ARCHIVE_EXTRACT_XATTR);
        SET_INT_MEMBER("EXTRACT_SECURE_SYMLINKS", ARCHIVE_EXTRACT_SECURE_SYMLINKS);
        SET_INT_MEMBER("EXTRACT_SECURE_NODOTDOT", ARCHIVE_EXTRACT_SECURE_NODOTDOT);
        SET_INT_MEMBER("EXTRACT_NO_AUTODIR", ARCHIVE_EXTRACT_NO_AUTODIR);
        SET_INT_MEMBER("EXTRACT_NO_OVERWRITE_NEWER", ARCHIVE_EXTRACT_NO_OVERWRITE_NEWER);
        SET_INT_MEMBER("EXTRACT_SPARSE", ARCHIVE_EXTRACT_SPARSE);
        SET_INT_MEMBER("EXTRACT_MAC_METADATA", ARCHIVE_EXTRACT_MAC_METADATA);
        SET_INT_MEMBER("EXTRACT_NO_HFS_COMPRESSION", ARCHIVE_EXTRACT_NO_HFS_COMPRESSION);
        SET_INT_MEMBER("EXTRACT_HFS_COMPRESSION_FORCED", ARCHIVE_EXTRACT_HFS_COMPRESSION_FORCED);
        SET_INT_MEMBER("EXTRACT_SECURE_NOABSOLUTEPATHS", ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS);
        SET_INT_MEMBER("EXTRACT_CLEAR_NOCHANGE_FFLAGS", ARCHIVE_EXTRACT_CLEAR_NOCHANGE_FFLAGS);
        SET_INT_MEMBER("EXTRACT_SAFE_WRITES", ARCHIVE_EXTRACT_SAFE_WRITES);

        _archive_module = module;
    }

    return _archive_module;
}
