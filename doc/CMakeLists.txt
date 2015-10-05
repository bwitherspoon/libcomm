# Copyright 2015 C. Brett Witherspoon

configure_file(Doxyfile.in Doxyfile @ONLY)
add_custom_target(
    doc
    ALL
    ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
