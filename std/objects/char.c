#include <unicase.h>
#include <unictype.h>

#include "builtins.h"

#define IS_PROPERTY_FN(name)                                                                       \
    static BuiltinResult _char_is_##name(int argc, UNUSED(Obj **argv), Obj *caller) {              \
        CHECK_ARG_COUNT(0)                                                                         \
        OK(new_bool(uc_is_property_##name(AS_CHAR(caller)->value)));                               \
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

static BuiltinResult _char_to_upper(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_char(uc_toupper(AS_CHAR(caller)->value)));
}

static BuiltinResult _char_to_lower(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_char(uc_tolower(AS_CHAR(caller)->value)));
}

static BuiltinResult _char_to_title(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)
    OK(new_char(uc_totitle(AS_CHAR(caller)->value)));
}

static BuiltinResult _char_to_decimal_digit(int argc, UNUSED(Obj **argv), Obj *caller) {
    CHECK_ARG_COUNT(0)

    int digit = uc_decimal_value(AS_CHAR(caller)->value);
    if (digit == -1) {
        ERR("Could not convert to digit.")
    } else {
        OK(new_int(digit));
    }
}

BuiltinTable *char_methods() {
    BuiltinTable *table = malloc(sizeof(BuiltinTable));
    init_method_table(table, 128);

    SET_BLTIN_METHOD("is_alphabetic", _char_is_alphabetic);
    SET_BLTIN_METHOD("is_ascii_hex_digit", _char_is_ascii_hex_digit);
    SET_BLTIN_METHOD("is_bidi_arabic_digit", _char_is_bidi_arabic_digit);
    SET_BLTIN_METHOD("is_bidi_arabic_right_to_left", _char_is_bidi_arabic_right_to_left);
    SET_BLTIN_METHOD("is_bidi_block_separator", _char_is_bidi_block_separator);
    SET_BLTIN_METHOD("is_bidi_boundary_neutral", _char_is_bidi_boundary_neutral);
    SET_BLTIN_METHOD("is_bidi_common_separator", _char_is_bidi_common_separator);
    SET_BLTIN_METHOD("is_bidi_control", _char_is_bidi_control);
    SET_BLTIN_METHOD("is_bidi_embedding_or_override", _char_is_bidi_embedding_or_override);
    SET_BLTIN_METHOD("is_bidi_european_digit", _char_is_bidi_european_digit);
    SET_BLTIN_METHOD("is_bidi_eur_num_separator", _char_is_bidi_eur_num_separator);
    SET_BLTIN_METHOD("is_bidi_eur_num_terminator", _char_is_bidi_eur_num_terminator);
    SET_BLTIN_METHOD("is_bidi_hebrew_right_to_left", _char_is_bidi_hebrew_right_to_left);
    SET_BLTIN_METHOD("is_bidi_left_to_right", _char_is_bidi_left_to_right);
    SET_BLTIN_METHOD("is_bidi_non_spacing_mark", _char_is_bidi_non_spacing_mark);
    SET_BLTIN_METHOD("is_bidi_other_neutral", _char_is_bidi_other_neutral);
    SET_BLTIN_METHOD("is_bidi_pdf", _char_is_bidi_pdf);
    SET_BLTIN_METHOD("is_bidi_segment_separator", _char_is_bidi_segment_separator);
    SET_BLTIN_METHOD("is_bidi_whitespace", _char_is_bidi_whitespace);
    SET_BLTIN_METHOD("is_cased", _char_is_cased);
    SET_BLTIN_METHOD("is_case_ignorable", _char_is_case_ignorable);
    SET_BLTIN_METHOD("is_changes_when_casefolded", _char_is_changes_when_casefolded);
    SET_BLTIN_METHOD("is_changes_when_casemapped", _char_is_changes_when_casemapped);
    SET_BLTIN_METHOD("is_changes_when_lowercased", _char_is_changes_when_lowercased);
    SET_BLTIN_METHOD("is_changes_when_titlecased", _char_is_changes_when_titlecased);
    SET_BLTIN_METHOD("is_changes_when_uppercased", _char_is_changes_when_uppercased);
    SET_BLTIN_METHOD("is_combining", _char_is_combining);
    SET_BLTIN_METHOD("is_composite", _char_is_composite);
    SET_BLTIN_METHOD("is_currency_symbol", _char_is_currency_symbol);
    SET_BLTIN_METHOD("is_dash", _char_is_dash);
    SET_BLTIN_METHOD("is_decimal_digit", _char_is_decimal_digit);
    SET_BLTIN_METHOD("is_default_ignorable_code_point", _char_is_default_ignorable_code_point);
    SET_BLTIN_METHOD("is_deprecated", _char_is_deprecated);
    SET_BLTIN_METHOD("is_diacritic", _char_is_diacritic);
    SET_BLTIN_METHOD("is_emoji", _char_is_emoji);
    SET_BLTIN_METHOD("is_emoji_component", _char_is_emoji_component);
    SET_BLTIN_METHOD("is_emoji_modifier", _char_is_emoji_modifier);
    SET_BLTIN_METHOD("is_emoji_modifier_base", _char_is_emoji_modifier_base);
    SET_BLTIN_METHOD("is_emoji_presentation", _char_is_emoji_presentation);
    SET_BLTIN_METHOD("is_extended_pictographic", _char_is_extended_pictographic);
    SET_BLTIN_METHOD("is_extender", _char_is_extender);
    SET_BLTIN_METHOD("is_format_control", _char_is_format_control);
    SET_BLTIN_METHOD("is_grapheme_base", _char_is_grapheme_base);
    SET_BLTIN_METHOD("is_grapheme_extend", _char_is_grapheme_extend);
    SET_BLTIN_METHOD("is_grapheme_link", _char_is_grapheme_link);
    SET_BLTIN_METHOD("is_hex_digit", _char_is_hex_digit);
    SET_BLTIN_METHOD("is_hyphen", _char_is_hyphen);
    SET_BLTIN_METHOD("is_ideographic", _char_is_ideographic);
    SET_BLTIN_METHOD("is_ids_binary_operator", _char_is_ids_binary_operator);
    SET_BLTIN_METHOD("is_ids_trinary_operator", _char_is_ids_trinary_operator);
    SET_BLTIN_METHOD("is_id_continue", _char_is_id_continue);
    SET_BLTIN_METHOD("is_id_start", _char_is_id_start);
    SET_BLTIN_METHOD("is_ignorable_control", _char_is_ignorable_control);
    SET_BLTIN_METHOD("is_iso_control", _char_is_iso_control);
    SET_BLTIN_METHOD("is_join_control", _char_is_join_control);
    SET_BLTIN_METHOD("is_left_of_pair", _char_is_left_of_pair);
    SET_BLTIN_METHOD("is_line_separator", _char_is_line_separator);
    SET_BLTIN_METHOD("is_logical_order_exception", _char_is_logical_order_exception);
    SET_BLTIN_METHOD("is_lowercase", _char_is_lowercase);
    SET_BLTIN_METHOD("is_math", _char_is_math);
    SET_BLTIN_METHOD("is_non_break", _char_is_non_break);
    SET_BLTIN_METHOD("is_not_a_character", _char_is_not_a_character);
    SET_BLTIN_METHOD("is_numeric", _char_is_numeric);
    SET_BLTIN_METHOD("is_other_alphabetic", _char_is_other_alphabetic);
    SET_BLTIN_METHOD(
        "is_other_default_ignorable_code_point", _char_is_other_default_ignorable_code_point);
    SET_BLTIN_METHOD("is_other_grapheme_extend", _char_is_other_grapheme_extend);
    SET_BLTIN_METHOD("is_other_id_continue", _char_is_other_id_continue);
    SET_BLTIN_METHOD("is_other_id_start", _char_is_other_id_start);
    SET_BLTIN_METHOD("is_other_lowercase", _char_is_other_lowercase);
    SET_BLTIN_METHOD("is_other_math", _char_is_other_math);
    SET_BLTIN_METHOD("is_other_uppercase", _char_is_other_uppercase);
    SET_BLTIN_METHOD("is_paired_punctuation", _char_is_paired_punctuation);
    SET_BLTIN_METHOD("is_paragraph_separator", _char_is_paragraph_separator);
    SET_BLTIN_METHOD("is_pattern_syntax", _char_is_pattern_syntax);
    SET_BLTIN_METHOD("is_pattern_white_space", _char_is_pattern_white_space);
    SET_BLTIN_METHOD("is_private_use", _char_is_private_use);
    SET_BLTIN_METHOD("is_punctuation", _char_is_punctuation);
    SET_BLTIN_METHOD("is_quotation_mark", _char_is_quotation_mark);
    SET_BLTIN_METHOD("is_radical", _char_is_radical);
    SET_BLTIN_METHOD("is_regional_indicator", _char_is_regional_indicator);
    SET_BLTIN_METHOD("is_sentence_terminal", _char_is_sentence_terminal);
    SET_BLTIN_METHOD("is_soft_dotted", _char_is_soft_dotted);
    SET_BLTIN_METHOD("is_space", _char_is_space);
    SET_BLTIN_METHOD("is_terminal_punctuation", _char_is_terminal_punctuation);
    SET_BLTIN_METHOD("is_titlecase", _char_is_titlecase);
    SET_BLTIN_METHOD("is_unassigned_code_value", _char_is_unassigned_code_value);
    SET_BLTIN_METHOD("is_unified_ideograph", _char_is_unified_ideograph);
    SET_BLTIN_METHOD("is_uppercase", _char_is_uppercase);
    SET_BLTIN_METHOD("is_variation_selector", _char_is_variation_selector);
    SET_BLTIN_METHOD("is_white_space", _char_is_white_space);
    SET_BLTIN_METHOD("is_xid_continue", _char_is_xid_continue);
    SET_BLTIN_METHOD("is_xid_start", _char_is_xid_start);
    SET_BLTIN_METHOD("is_zero_width", _char_is_zero_width);
    SET_BLTIN_METHOD("to_upper", _char_to_upper);
    SET_BLTIN_METHOD("to_lower", _char_to_lower);
    SET_BLTIN_METHOD("to_title", _char_to_title);
    SET_BLTIN_METHOD("to_decimal_digit", _char_to_decimal_digit);

    return table;
}
