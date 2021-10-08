INCLUDE(${CMAKE_CURRENT_LIST_DIR}/impl/InjectorImpl.cmake)
INCLUDE(CMakeParseArguments)

FUNCTION(__RES_INJ_PROCESS_RESOURCE TARGET GENERATED_DIR PATH NAME IS_CT)
    IF (IS_CT STREQUAL TRUE)
        __RES_INJ_TARGET_INJECT_CONSTEXPR_RESOURCE(${TARGET} ${NAME} ${PATH})
    ELSE()
        __RES_INJ_TARGET_INJECT_RESOURCE(${TARGET} ${NAME} ${PATH} ${GENERATED_DIR})
    ENDIF()
ENDFUNCTION()


FUNCTION(__RES_INJ_PROCESS_RESOURCES TARGET BASE_RESOURCE_PATH GENERATED_DIR ARGS IS_CT)
    LIST(LENGTH ARGS LEN)
    SET(IDX 0)

    MACRO(INVALID_SYNTAX)
        MATH(EXPR LEN-POS "${LEN} - ${IDX}")
        LIST(SUBLIST ARGS ${IDX} ${LEN-POS} ARGS)
        STRING(REPLACE ";" " " TEXT "${ARGS}")
        MESSAGE(FATAL_ERROR "[INJECT_RESOURCES] invalid syntax: '${TEXT}'")
    ENDMACRO()

    WHILE (IDX LESS LEN)
        MATH(EXPR IDX+1 "${IDX} + 1")
        MATH(EXPR IDX+2 "${IDX} + 2")
        MATH(EXPR IDX+3 "${IDX} + 3")

        IF (IDX+3 GREATER LEN)
            INVALID_SYNTAX()
        ENDIF()

        LIST(GET ARGS ${IDX} PATH)
        LIST(GET ARGS ${IDX+1} AS)
        LIST(GET ARGS ${IDX+2} NAME)

        IF (NOT "${AS}" STREQUAL "AS")
            INVALID_SYNTAX()
        ENDIF()

        __RES_INJ_PROCESS_RESOURCE(
                ${TARGET} ${GENERATED_DIR} ${BASE_RESOURCE_PATH}/${PATH} ${NAME} ${IS_CT})

        SET(IDX ${IDX+3})
    ENDWHILE()
ENDFUNCTION()


MACRO(__INJ_RES_PROCESS_UPPER_LOWER PREFIX NAME)
    STRING(TOUPPER ${NAME} __UPPER)
    STRING(TOLOWER ${NAME} __LOWER)
    IF (DEFINED ${PREFIX}_${__LOWER})
        IF (DEFINED ${PREFIX}_${__UPPER})
            MESSAGE(FATAL_ERROR "[INJECT_RESOURCES] Property '${__UPPER}' conflicts with '${__LOWER}'")
        ELSE()
            SET(${PREFIX}_${__UPPER} "${${PREFIX}_${__LOWER}}")
        ENDIF()
    ENDIF()
ENDMACRO( )


FUNCTION(INJECT_RESOURCES)
    CMAKE_PARSE_ARGUMENTS(
        INPUT
        ""
        "TARGET;target;GENERATED_DIR;generated_dir;RESOURCES_DIR;resources_dir"
        "RESOURCES;resources;COMPILE_TIME_RESOURCES;compile_time_resources"
        ${ARGN}
    )

    IF (DEFINED INPUT_UNPARSED_ARGUMENTS)
        LIST(GET INPUT_UNPARSED_ARGUMENTS 0 ARG)
        MESSAGE(FATAL_ERROR "[INJECT_RESOURCES] unknown argument: ${ARG}")
    ENDIF()

    __INJ_RES_PROCESS_UPPER_LOWER(INPUT TARGET)
    __INJ_RES_PROCESS_UPPER_LOWER(INPUT GENERATED_DIR)
    __INJ_RES_PROCESS_UPPER_LOWER(INPUT RESOURCES_DIR)
    __INJ_RES_PROCESS_UPPER_LOWER(INPUT RESOURCES)
    __INJ_RES_PROCESS_UPPER_LOWER(INPUT COMPILE_TIME_RESOURCES)

    IF (NOT INPUT_TARGET)
        MESSAGE(FATAL_ERROR "[INJECT_RESOURCES] target is not specified")
    ENDIF()
    IF (NOT INPUT_GENERATED_DIR)
        SET(INPUT_GENERATED_DIR ${PROJECT_BINARY_DIR}/generated)
    ENDIF()
    IF (NOT INPUT_RESOURCES_DIR)
        SET(INPUT_RESOURCES_DIR ${CMAKE_SOURCE_DIR})
    ENDIF()

    __RES_INJ_PROCESS_RESOURCES(
            ${INPUT_TARGET} ${INPUT_RESOURCES_DIR} ${INPUT_GENERATED_DIR}
            "${INPUT_RESOURCES}" FALSE)


    __RES_INJ_PROCESS_RESOURCES(
            ${INPUT_TARGET} ${INPUT_RESOURCES_DIR} ${INPUT_GENERATED_DIR}
            "${INPUT_COMPILE_TIME_RESOURCES}" TRUE)
ENDFUNCTION()