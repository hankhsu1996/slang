"""Bazel rule for generating export headers similar to CMake's generate_export_header."""

def _generate_export_header_impl(ctx):
    """Implements export_header rule by generating a C++ export header file.

    Creates a header file with proper export/import macros for building shared/static libraries,
    similar to CMake's generate_export_header() function.
    """
    base_name = ctx.attr.base_name

    # Generate the export header content
    header_content = """#ifndef {base_name_upper}_EXPORT_H
#define {base_name_upper}_EXPORT_H

#ifdef {base_name_upper}_STATIC_DEFINE
#  define {base_name_upper}_EXPORT
#  define {base_name_upper}_NO_EXPORT
#else
#  ifndef {base_name_upper}_EXPORT
#    ifdef slang_slang_EXPORTS
        /* We are building this library */
#      define {base_name_upper}_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define {base_name_upper}_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef {base_name_upper}_NO_EXPORT
#    define {base_name_upper}_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef {base_name_upper}_DEPRECATED
#  define {base_name_upper}_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef {base_name_upper}_DEPRECATED_EXPORT
#  define {base_name_upper}_DEPRECATED_EXPORT {base_name_upper}_EXPORT {base_name_upper}_DEPRECATED
#endif

#ifndef {base_name_upper}_DEPRECATED_NO_EXPORT
#  define {base_name_upper}_DEPRECATED_NO_EXPORT {base_name_upper}_NO_EXPORT {base_name_upper}_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef {base_name_upper}_NO_DEPRECATED
#    define {base_name_upper}_NO_DEPRECATED
#  endif
#endif

{custom_content}

#endif /* {base_name_upper}_EXPORT_H */
""".format(
        base_name_upper = base_name.upper(),
        custom_content = ctx.attr.custom_content,
    )

    # Write the header file
    ctx.actions.write(
        output = ctx.outputs.out,
        content = header_content,
    )

# Rule definition
export_header = rule(
    implementation = _generate_export_header_impl,
    attrs = {
        "base_name": attr.string(
            mandatory = True,
            doc = "Base name for the export macros (e.g., 'slang' generates SLANG_EXPORT)",
        ),
        "export_file_name": attr.string(
            mandatory = True,
            doc = "Name of the output export header file",
        ),
        "custom_content": attr.string(
            default = "",
            doc = "Custom content to include in the header file",
        ),
        "out": attr.output(
            mandatory = True,
            doc = "Output export header file",
        ),
    },
    doc = "Generates a C++ export header file with visibility macros",
)
