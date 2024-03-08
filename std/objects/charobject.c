#include <unicase.h>
#include <unictype.h>

#include "builtins.h"

#define IS_PROPERTY_FN(name)                                                                       \
    BuiltinResult _char_is_##name(int argc, UNUSED(Obj **, argv), Obj *caller) {                   \
        CHECK_ARG_COUNT(0)                                                                         \
        return OK(new_bool(uc_is_property_##name(AS_CHAR(caller)->value)));                        \
    }

IS_PROPERTY_FN(alphabetic)
IS_PROPERTY_FN(ascii_hex_digit)
IS_PROPERTY_FN(bidi_arabic_digit)
IS_PROPERTY_FN(bidi_arabic_right_to_left)
IS_PROPERTY_FN(bidi_block_separator)
IS_PROPERTY_FN(bidi_boundary_neutral)
IS_PROPERTY_FN(bidi_common_separator)
IS_PROPERTY_FN(bidi_control)
IS_PROPERTY_FN(bidi_embedding_or_override)
IS_PROPERTY_FN(bidi_european_digit)
IS_PROPERTY_FN(bidi_eur_num_separator)
IS_PROPERTY_FN(bidi_eur_num_terminator)
IS_PROPERTY_FN(bidi_hebrew_right_to_left)
IS_PROPERTY_FN(bidi_left_to_right)
IS_PROPERTY_FN(bidi_non_spacing_mark)
IS_PROPERTY_FN(bidi_other_neutral)
IS_PROPERTY_FN(bidi_pdf)
IS_PROPERTY_FN(bidi_segment_separator)
IS_PROPERTY_FN(bidi_whitespace)
IS_PROPERTY_FN(cased)
IS_PROPERTY_FN(case_ignorable)
IS_PROPERTY_FN(changes_when_casefolded)
IS_PROPERTY_FN(changes_when_casemapped)
IS_PROPERTY_FN(changes_when_lowercased)
IS_PROPERTY_FN(changes_when_titlecased)
IS_PROPERTY_FN(changes_when_uppercased)
IS_PROPERTY_FN(combining)
IS_PROPERTY_FN(composite)
IS_PROPERTY_FN(currency_symbol)
IS_PROPERTY_FN(dash)
IS_PROPERTY_FN(decimal_digit)
IS_PROPERTY_FN(default_ignorable_code_point)
IS_PROPERTY_FN(deprecated)
IS_PROPERTY_FN(diacritic)
IS_PROPERTY_FN(emoji)
IS_PROPERTY_FN(emoji_component)
IS_PROPERTY_FN(emoji_modifier)
IS_PROPERTY_FN(emoji_modifier_base)
IS_PROPERTY_FN(emoji_presentation)
IS_PROPERTY_FN(extended_pictographic)
IS_PROPERTY_FN(extender)
IS_PROPERTY_FN(format_control)
IS_PROPERTY_FN(grapheme_base)
IS_PROPERTY_FN(grapheme_extend)
IS_PROPERTY_FN(grapheme_link)
IS_PROPERTY_FN(hex_digit)
IS_PROPERTY_FN(hyphen)
IS_PROPERTY_FN(ideographic)
IS_PROPERTY_FN(ids_binary_operator)
IS_PROPERTY_FN(ids_trinary_operator)
IS_PROPERTY_FN(id_continue)
IS_PROPERTY_FN(id_start)
IS_PROPERTY_FN(ignorable_control)
IS_PROPERTY_FN(iso_control)
IS_PROPERTY_FN(join_control)
IS_PROPERTY_FN(left_of_pair)
IS_PROPERTY_FN(line_separator)
IS_PROPERTY_FN(logical_order_exception)
IS_PROPERTY_FN(lowercase)
IS_PROPERTY_FN(math)
IS_PROPERTY_FN(non_break)
IS_PROPERTY_FN(not_a_character)
IS_PROPERTY_FN(numeric)
IS_PROPERTY_FN(other_alphabetic)
IS_PROPERTY_FN(other_default_ignorable_code_point)
IS_PROPERTY_FN(other_grapheme_extend)
IS_PROPERTY_FN(other_id_continue)
IS_PROPERTY_FN(other_id_start)
IS_PROPERTY_FN(other_lowercase)
IS_PROPERTY_FN(other_math)
IS_PROPERTY_FN(other_uppercase)
IS_PROPERTY_FN(paired_punctuation)
IS_PROPERTY_FN(paragraph_separator)
IS_PROPERTY_FN(pattern_syntax)
IS_PROPERTY_FN(pattern_white_space)
IS_PROPERTY_FN(private_use)
IS_PROPERTY_FN(punctuation)
IS_PROPERTY_FN(quotation_mark)
IS_PROPERTY_FN(radical)
IS_PROPERTY_FN(regional_indicator)
IS_PROPERTY_FN(sentence_terminal)
IS_PROPERTY_FN(soft_dotted)
IS_PROPERTY_FN(space)
IS_PROPERTY_FN(terminal_punctuation)
IS_PROPERTY_FN(titlecase)
IS_PROPERTY_FN(unassigned_code_value)
IS_PROPERTY_FN(unified_ideograph)
IS_PROPERTY_FN(uppercase)
IS_PROPERTY_FN(variation_selector)
IS_PROPERTY_FN(white_space)
IS_PROPERTY_FN(xid_continue)
IS_PROPERTY_FN(xid_start)
IS_PROPERTY_FN(zero_width)

BuiltinResult _char_to_upper(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_toupper(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_lower(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_tolower(AS_CHAR(caller)->value)));
}

BuiltinResult _char_to_title(int argc, UNUSED(Obj **, argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    return OK(take_char(uc_totitle(AS_CHAR(caller)->value)));
}

BuiltinTable *char_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 128);

    SET_BLTIN_METHOD(char, is_alphabetic);
    SET_BLTIN_METHOD(char, is_ascii_hex_digit);
    SET_BLTIN_METHOD(char, is_bidi_arabic_digit);
    SET_BLTIN_METHOD(char, is_bidi_arabic_right_to_left);
    SET_BLTIN_METHOD(char, is_bidi_block_separator);
    SET_BLTIN_METHOD(char, is_bidi_boundary_neutral);
    SET_BLTIN_METHOD(char, is_bidi_common_separator);
    SET_BLTIN_METHOD(char, is_bidi_control);
    SET_BLTIN_METHOD(char, is_bidi_embedding_or_override);
    SET_BLTIN_METHOD(char, is_bidi_european_digit);
    SET_BLTIN_METHOD(char, is_bidi_eur_num_separator);
    SET_BLTIN_METHOD(char, is_bidi_eur_num_terminator);
    SET_BLTIN_METHOD(char, is_bidi_hebrew_right_to_left);
    SET_BLTIN_METHOD(char, is_bidi_left_to_right);
    SET_BLTIN_METHOD(char, is_bidi_non_spacing_mark);
    SET_BLTIN_METHOD(char, is_bidi_other_neutral);
    SET_BLTIN_METHOD(char, is_bidi_pdf);
    SET_BLTIN_METHOD(char, is_bidi_segment_separator);
    SET_BLTIN_METHOD(char, is_bidi_whitespace);
    SET_BLTIN_METHOD(char, is_cased);
    SET_BLTIN_METHOD(char, is_case_ignorable);
    SET_BLTIN_METHOD(char, is_changes_when_casefolded);
    SET_BLTIN_METHOD(char, is_changes_when_casemapped);
    SET_BLTIN_METHOD(char, is_changes_when_lowercased);
    SET_BLTIN_METHOD(char, is_changes_when_titlecased);
    SET_BLTIN_METHOD(char, is_changes_when_uppercased);
    SET_BLTIN_METHOD(char, is_combining);
    SET_BLTIN_METHOD(char, is_composite);
    SET_BLTIN_METHOD(char, is_currency_symbol);
    SET_BLTIN_METHOD(char, is_dash);
    SET_BLTIN_METHOD(char, is_decimal_digit);
    SET_BLTIN_METHOD(char, is_default_ignorable_code_point);
    SET_BLTIN_METHOD(char, is_deprecated);
    SET_BLTIN_METHOD(char, is_diacritic);
    SET_BLTIN_METHOD(char, is_emoji);
    SET_BLTIN_METHOD(char, is_emoji_component);
    SET_BLTIN_METHOD(char, is_emoji_modifier);
    SET_BLTIN_METHOD(char, is_emoji_modifier_base);
    SET_BLTIN_METHOD(char, is_emoji_presentation);
    SET_BLTIN_METHOD(char, is_extended_pictographic);
    SET_BLTIN_METHOD(char, is_extender);
    SET_BLTIN_METHOD(char, is_format_control);
    SET_BLTIN_METHOD(char, is_grapheme_base);
    SET_BLTIN_METHOD(char, is_grapheme_extend);
    SET_BLTIN_METHOD(char, is_grapheme_link);
    SET_BLTIN_METHOD(char, is_hex_digit);
    SET_BLTIN_METHOD(char, is_hyphen);
    SET_BLTIN_METHOD(char, is_ideographic);
    SET_BLTIN_METHOD(char, is_ids_binary_operator);
    SET_BLTIN_METHOD(char, is_ids_trinary_operator);
    SET_BLTIN_METHOD(char, is_id_continue);
    SET_BLTIN_METHOD(char, is_id_start);
    SET_BLTIN_METHOD(char, is_ignorable_control);
    SET_BLTIN_METHOD(char, is_iso_control);
    SET_BLTIN_METHOD(char, is_join_control);
    SET_BLTIN_METHOD(char, is_left_of_pair);
    SET_BLTIN_METHOD(char, is_line_separator);
    SET_BLTIN_METHOD(char, is_logical_order_exception);
    SET_BLTIN_METHOD(char, is_lowercase);
    SET_BLTIN_METHOD(char, is_math);
    SET_BLTIN_METHOD(char, is_non_break);
    SET_BLTIN_METHOD(char, is_not_a_character);
    SET_BLTIN_METHOD(char, is_numeric);
    SET_BLTIN_METHOD(char, is_other_alphabetic);
    SET_BLTIN_METHOD(char, is_other_default_ignorable_code_point);
    SET_BLTIN_METHOD(char, is_other_grapheme_extend);
    SET_BLTIN_METHOD(char, is_other_id_continue);
    SET_BLTIN_METHOD(char, is_other_id_start);
    SET_BLTIN_METHOD(char, is_other_lowercase);
    SET_BLTIN_METHOD(char, is_other_math);
    SET_BLTIN_METHOD(char, is_other_uppercase);
    SET_BLTIN_METHOD(char, is_paired_punctuation);
    SET_BLTIN_METHOD(char, is_paragraph_separator);
    SET_BLTIN_METHOD(char, is_pattern_syntax);
    SET_BLTIN_METHOD(char, is_pattern_white_space);
    SET_BLTIN_METHOD(char, is_private_use);
    SET_BLTIN_METHOD(char, is_punctuation);
    SET_BLTIN_METHOD(char, is_quotation_mark);
    SET_BLTIN_METHOD(char, is_radical);
    SET_BLTIN_METHOD(char, is_regional_indicator);
    SET_BLTIN_METHOD(char, is_sentence_terminal);
    SET_BLTIN_METHOD(char, is_soft_dotted);
    SET_BLTIN_METHOD(char, is_space);
    SET_BLTIN_METHOD(char, is_terminal_punctuation);
    SET_BLTIN_METHOD(char, is_titlecase);
    SET_BLTIN_METHOD(char, is_unassigned_code_value);
    SET_BLTIN_METHOD(char, is_unified_ideograph);
    SET_BLTIN_METHOD(char, is_uppercase);
    SET_BLTIN_METHOD(char, is_variation_selector);
    SET_BLTIN_METHOD(char, is_white_space);
    SET_BLTIN_METHOD(char, is_xid_continue);
    SET_BLTIN_METHOD(char, is_xid_start);
    SET_BLTIN_METHOD(char, is_zero_width);
    SET_BLTIN_METHOD(char, to_upper);
    SET_BLTIN_METHOD(char, to_lower);
    SET_BLTIN_METHOD(char, to_title);

    return table;
}
