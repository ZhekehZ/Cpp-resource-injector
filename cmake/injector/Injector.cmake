ENABLE_LANGUAGE(ASM)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/compiler-dependent/GetASM.cmake)
GET_ASM(GENERIC_ASM_FILE)

SET(CMAKE_RESOURCE_INJECTOR_PREFIX "CMAKE_RESOURCE_INJECTOR_PREFIX")
SET(RESOURCE_INJECTOR_PATH ..)

FUNCTION(TARGET_INJECT_RESOURCE TARGET RES_NAME PATH GENERATED_DIR IS_CONSTEXPR)
    SET(PREFIX ${CMAKE_RESOURCE_INJECTOR_PREFIX}_TARGET_${TARGET})
    SET(SOURCE_PREFIX ${CMAKE_RESOURCE_INJECTOR_PREFIX})

    IF (NOT EXISTS ${PATH})
        MESSAGE(FATAL_ERROR "FILE '${PATH}' IS NOT EXISTS")
    ENDIF()
    SET(RES_PATH ${PATH})

    IF (${IS_CONSTEXPR})
        FILE(READ ${PATH} TEXT)
        STRING(REPLACE ";" "\\\;" TEXT "${TEXT}")
        STRING(REPLACE "\n" "\\n" TEXT "${TEXT}")
        TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE ${SOURCE_PREFIX}_${RES_NAME}="${TEXT}")

        SET(NEW_ENUM "${RES_NAME}")
        SET(NEW_ENUM_USAGE
            "template <>                                                                               \
            consteval int ___compile_time_data_size<constinit_injected_resources::${RES_NAME}>() {     \
                return sizeof( \"${TEXT}\" )\\\;                                                       \
            }                                                                                          \
                                                                                                       \
            template <>                                                                                \
            consteval char const * ___compile_time_data<constinit_injected_resources::${RES_NAME}>() { \
                return \"${TEXT}\"\\\;                                                                 \
            }")

        EXTEND_VAR(ENUM_CONSTEXPR , NEW_ENUM)
        EXTEND_VAR(IMPLEMENTATION_CONSTINIT " " NEW_ENUM_USAGE)

        RETURN()
    ENDIF()

    IF (NOT DEFINED ${PREFIX}_INJECTIONS_COUNTER)
        SET(${PREFIX}_INJECTIONS_COUNTER 0 PARENT_SCOPE)
        SET(COUNTER 0)
    ELSE()
        SET(COUNTER ${${PREFIX}_INJECTIONS_COUNTER})
    ENDIF()

    FUNCTION(TARGET_REMOVE_DEFINITION TARGET VAR)
        GET_TARGET_PROPERTY(DEFS ${TARGET} COMPILE_DEFINITIONS)
        STRING(REPLACE "\;" "<SEMICOLON>" DEFS_1 "${DEFS}")
        LIST(FILTER DEFS_1 EXCLUDE REGEX "^${VAR}=")
        STRING(REPLACE "<SEMICOLON>" "\;" DEFS "${DEFS_1}")
        SET_PROPERTY(TARGET ${TARGET} PROPERTY COMPILE_DEFINITIONS "${DEFS}")
    ENDFUNCTION()

    MACRO(EXTEND_VAR VAR SEPARATOR NEW_VAL_NAME)
        SET(NEW_VAL ${${NEW_VAL_NAME}})
        STRING(REPLACE "\;" "\\\;" NEW_VAL "${NEW_VAL}")

        IF ("${SEPARATOR}" STREQUAL \;)
            SET(SEP "\\\;")
        ELSE()
            SET(SEP "${SEPARATOR}")
        ENDIF()

        MATH(EXPR PREV_IDX "${COUNTER} - 1")
        WHILE(NOT DEFINED ${PREFIX}_${VAR}_${PREV_IDX})
            IF (PREV_IDX LESS 0)
                BREAK()
            ENDIF()
            MATH(EXPR PREV_IDX "${PREV_IDX} - 1")
        ENDWHILE()

        IF (PREV_IDX GREATER -1)
            TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
                    ${SOURCE_PREFIX}_${VAR}_${COUNTER}=${SOURCE_PREFIX}_${VAR}_${PREV_IDX}${SEP}${NEW_VAL})
        ELSE()
            TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE
                    ${SOURCE_PREFIX}_${VAR}_${COUNTER}=${NEW_VAL})
        ENDIF()
        SET(${PREFIX}_${VAR}_${COUNTER} TRUE PARENT_SCOPE)

        TARGET_REMOVE_DEFINITION(${TARGET} ${SOURCE_PREFIX}_${VAR})
        TARGET_COMPILE_DEFINITIONS(${TARGET} PRIVATE ${SOURCE_PREFIX}_${VAR}=${SOURCE_PREFIX}_${VAR}_${COUNTER})
    ENDMACRO()

    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_DIR})

    SET(NEW_ENUM "${RES_NAME}")
    SET(NEW_ENUM_USAGE
        "template <>                                                                   \
        int ___compile_time_data_size<injector::injected_resources::${RES_NAME}>() {     \
            extern const int ${RES_NAME}_size\\\;                                      \
            return ${RES_NAME}_size\\\;                                                \
        }                                                                              \
                                                                                       \
        template <>                                                                    \
        char const * ___compile_time_data<injector::injected_resources::${RES_NAME}>() { \
            extern const char ${RES_NAME}_data[]\\\;                                   \
            return ${RES_NAME}_data\\\;                                                \
        }")

    EXTEND_VAR(ENUM , NEW_ENUM)
    EXTEND_VAR(IMPLEMENTATION " " NEW_ENUM_USAGE)

    MATH(EXPR COUNTER "${COUNTER} + 1")

    SET(GENERATED_PATH "${GENERATED_DIR}/${RES_NAME}.S")
    CONFIGURE_FILE(${GENERIC_ASM_FILE} ${GENERATED_PATH}.tmp)  # RES_NAME, RES_PATH

    ADD_CUSTOM_COMMAND(OUTPUT ${GENERATED_PATH}
            COMMAND ${CMAKE_COMMAND} -E copy ${GENERATED_PATH}.tmp ${GENERATED_PATH}
            COMMAND ${CMAKE_COMMAND} -E remove ${GENERATED_PATH}.tmp
            DEPENDS ${PATH})

    TARGET_SOURCES(${TARGET} PUBLIC ${GENERATED_PATH})

    SET(${PREFIX}_INJECTIONS_COUNTER ${COUNTER} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(INJECT_RESOURCES)
    IF (ARGC LESS 2 OR NOT ARGV0 STREQUAL TARGET)
        MESSAGE(FATAL_ERROR "INJECT_RESOURCES: TARGET IS NOT DEFINED")
    ENDIF()
    SET(TARGET ${ARGV1})

    SET(COUNTER 2)
    IF (ARGC EQUAL COUNTER)
        RETURN()
    ENDIF()

    SET(GENERATE_DIR generated)
    IF (ARGV${COUNTER} STREQUAL "GENERATE_DIR")
        MATH(EXPR COUNTER "${COUNTER} + 1")
        IF (ARGC EQUAL COUNTER)
            MESSAGE(FATAL_ERROR "INJECT_RESOURCES: DIR EXPECTED")
        ENDIF()
        SET(GENERATE_DIR ${ARGV${COUNTER}})
        MATH(EXPR COUNTER "${COUNTER} + 1")
    ENDIF()

    SET(IS_CONSTEXPR "")
    WHILE (TRUE)
        IF (ARGC EQUAL COUNTER)
            RETURN()
        ENDIF()

        IF (ARGV${COUNTER} STREQUAL "RESOURCES")
            MATH(EXPR COUNTER "${COUNTER} + 1")
            SET(IS_CONSTEXPR FALSE)
        ELSEIF(ARGV${COUNTER} STREQUAL "COMPILE_TIME_RESOURCES")
            MATH(EXPR COUNTER "${COUNTER} + 1")
            SET(IS_CONSTEXPR TRUE)
        ENDIF()

        IF (IS_CONSTEXPR STREQUAL "")
            MESSAGE(FATAL_ERROR "INJECT_RESOURCES: RESOURCE TYPE (RESOURCES/COMPILE_TIME_RESOURCES) "
                                "NOT SPECIFIED")
        ENDIF()

        IF (ARGC EQUAL COUNTER)
            MESSAGE(FATAL_ERROR "INJECT_RESOURCES: RESOURCE INFO EXPECTED")
        ENDIF()

        SET(RESOURCE_PATH ${ARGV${COUNTER}})
        MATH(EXPR AS_IDX "${COUNTER} + 1")
        MATH(EXPR NAME_IDX "${COUNTER} + 2")

        IF (NAME_IDX GREATER_EQUAL ARGC)
            MESSAGE(FATAL_ERROR "INJECT_RESOURCES: INVALID RESOURCE INFO FORMAT")
        ENDIF()

        IF (NOT ARGV${AS_IDX} STREQUAL AS)
            MESSAGE(FATAL_ERROR "INJECT_RESOURCES: 'AS' KEYWORD EXPECTED")
        ENDIF()

        SET(NAME ${ARGV${NAME_IDX}})
        TARGET_INJECT_RESOURCE(${TARGET} ${NAME} ${RESOURCE_PATH} ${GENERATE_DIR} ${IS_CONSTEXPR})

        MATH(EXPR COUNTER "${COUNTER} + 3")
    ENDWHILE()
ENDFUNCTION()