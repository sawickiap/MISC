# FP8 Tables
#
# A script that generates HTML code with tables visualizing all the possible
# values of 8-bit floating-point (FP8) data types used in machine learning.
# Developed to generate tables for the article:
#
# <LINK TBD>
#
# Author:  Adam Sawicki, https://asawicki.info, adam__REMOVE__@asawicki.info
# Version: 1.0
# License: Public Domain

import math
from enum import Enum

class FP8Type(Enum):
    FLOAT8E4M3FN = 1
    FLOAT8E4M3FNUZ = 2
    FLOAT8E5M2 = 3
    FLOAT8E5M2FNUZ = 4

html_begin = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>FP8 Tables</title>
    <style>
        table.fp8 {
            border-collapse: collapse;
            border: 2px solid #fff;
        }
        table.fp8 td, table.fp8 th {
            border: 2px solid #fff;
            padding: 2px;
            font-size: 0.95em;
        }
        table.fp8 th {
            text-align: center;
        }
        table.fp8 td {
            text-align: right;
        }
        table.fp8 td.denorm {
            font-style: italic;
            color: #888;
        }
        table.fp8 td.ordinal {
            color: #44f;
            font-weight: bold;
        }
    </style>
</head>
<body>
"""
html_end = """</body>
</html>
"""

BKG_COLOR_ZERO = '#ff4'
BKG_COLOR_NAN = '#04eff8'
BKG_COLOR_POS_INF = '#4f4'
BKG_COLOR_NEG_INF = '#f44'

BKG_COLOR_BASE = (255/255, 235/255, 132/255)
BKG_COLOR_MAX_POS = (99/255, 190/255, 123/255)
BKG_COLOR_MAX_NEG = (248/255, 105/255, 107/255)

FONT_COLOR_SIGN = '#c00'
FONT_COLOR_EXPONENT = '#080'
FONT_COLOR_MANTISSA = '#aaa'

def interpolate_color(color_tuple_a, color_tuple_b, t):
    a_linear = (color_tuple_a[0]**2.2, color_tuple_a[1]**2.2, color_tuple_a[2]**2.2)
    b_linear = (color_tuple_b[0]**2.2, color_tuple_b[1]**2.2, color_tuple_b[2]**2.2)
    return ((a_linear[0] + (b_linear[0] - a_linear[0]) * t) ** (1/2.2),
        (a_linear[1] + (b_linear[1] - a_linear[1]) * t) ** (1/2.2),
        (a_linear[2] + (b_linear[2] - a_linear[2]) * t) ** (1/2.2))

def interpolate_format_color(color_tuple_a, color_tuple_b, t):
    c = interpolate_color(color_tuple_a, color_tuple_b, t)
    r = min(max(0, math.floor(c[0] * 255)), 255)
    g = min(max(0, math.floor(c[1] * 255)), 255)
    b = min(max(0, math.floor(c[2] * 255)), 255)
    r = hex(r)[2:].zfill(2)
    g = hex(g)[2:].zfill(2)
    b = hex(b)[2:].zfill(2)
    return f'#{r}{g}{b}'

def format_color(color_tuple):
    r = min(max(0, math.floor(color_tuple[0] * 255)), 255)
    g = min(max(0, math.floor(color_tuple[1] * 255)), 255)
    b = min(max(0, math.floor(color_tuple[2] * 255)), 255)
    r = hex(r)[2:].zfill(2)
    g = hex(g)[2:].zfill(2)
    b = hex(b)[2:].zfill(2)
    return f'#{r}{g}{b}'

LEGEND = "<table class=\"fp8\">" + \
    f"<tr><td style=\"background-color:{BKG_COLOR_ZERO}\">+0<td style=\"text-align:left;\">Saturated yellow - zero\n" + \
    f"<tr><td style=\"background-color:{format_color(BKG_COLOR_BASE)}\">0.01562<td style=\"text-align:left;\">Yellow - small numbers, close to zero\n" + \
    f"<tr><td style=\"background-color:{format_color(BKG_COLOR_MAX_POS)}\">448<td style=\"text-align:left;\">Green - larger positive numbers\n" + \
    f"<tr><td style=\"background-color:{format_color(BKG_COLOR_MAX_NEG)}\">-448<td style=\"text-align:left;\">Red - larger negative numbers\n" + \
    f"<tr><td class=\"denorm\">0.001953<td style=\"text-align:left;\">Gray & italic - denormialized number (no implicit \"1.\")\n" + \
    f"<tr><td class=\"ordinal\">12<td style=\"text-align:left;\">Blue & bold - subsequent integer numbers (smaller numbers support fractions, larger ones start jumping every 2, 4, 8...)\n" + \
    f"<tr><td style=\"background-color:{BKG_COLOR_NAN}\">NaN<td style=\"text-align:left;\">Teal - not a number (NaN)\n" + \
    f"<tr><td style=\"background-color:{BKG_COLOR_POS_INF}\">+INF<td style=\"text-align:left;\">Saturated green - positive infinity\n" + \
    f"<tr><td style=\"background-color:{BKG_COLOR_NEG_INF}\">-INF<td style=\"text-align:left;\">Saturated red - negative infinity\n" + \
    "</table>\n"

CAPS = "<table>\n" + \
    "<tr><th>&nbsp;<th>FLOAT8E4M3FN<th>FLOAT8E4M3FNUZ<th>FLOAT8E5M2<th>FLOAT8E5M2FNUZ\n" + \
    "<tr><td>Minimum positive subnormal<td style=\"text-align:right\">0.001953<td style=\"text-align:right\">0.0009766<td style=\"text-align:right\">0.0000153<td style=\"text-align:right\">0.0000076\n" + \
    "<tr><td>Minimum positive normal<td style=\"text-align:right\">0.01562<td style=\"text-align:right\">0.007812<td style=\"text-align:right\">0.000061<td style=\"text-align:right\">0.0000305\n" + \
    "<tr><td>Next value after 1<td style=\"text-align:right\">1.125<td style=\"text-align:right\">1.125<td style=\"text-align:right\">1.25<td style=\"text-align:right\">1.25\n" + \
    "<tr><td>Maximum integer represented exactly<td style=\"text-align:right\">16<td style=\"text-align:right\">16<td style=\"text-align:right\">8<td style=\"text-align:right\">8\n" + \
    "<tr><td>Maximum<td style=\"text-align:right\">448<td style=\"text-align:right\">240<td style=\"text-align:right\">57344<td style=\"text-align:right\">57344\n" + \
    "<tr>\n" + \
       "<td>0\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">S</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">0000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">000</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">0</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">0000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">000</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">S</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">00000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">00</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">0</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">00000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">00</span>\n" + \
    f"<tr>0\n" + \
       f"<td>NaN<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">S</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">1111</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">111</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">1</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">0000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">000</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">S</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">11111</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">MM</span>\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">1</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">00000</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">00</span>\n" + \
    f"<tr>0\n" + \
       f"<td>INF<td style=\"text-align:right\">&nbsp;\n" + \
       f"<td style=\"text-align:right\">&nbsp;\n" + \
       f"<td style=\"text-align:right\"><span style=\"color:{FONT_COLOR_SIGN}\">S</span> <span style=\"color:{FONT_COLOR_EXPONENT}\">11111</span> <span style=\"color:{FONT_COLOR_MANTISSA}\">00</span>\n" + \
       f"<td style=\"text-align:right\">&nbsp;\n" + \
    "</table>\n"


def format_value(value: float) -> str:
    if value < 0.001:
        s = f'{value:.7f}'
    elif value < 0.01:
        s = f'{value:.6f}'
    elif value < 0.1:
        s = f'{value:.5f}'
    elif value < 1:
        s = f'{value:.4f}'
    else:
        s = f'{value}'
    # When decimal number is present, remove trailing zeros, like "2.5000".
    if '.' in s:
        while s.endswith('0'):
            s = s[:len(s)-1]
        # Remove trailing dot, like "2.".
        if s.endswith('.'):
            s = s[:len(s) - 1]
    return s

def is_ordinal(type: FP8Type, value: float) -> bool:
    if type == FP8Type.FLOAT8E4M3FN or type == FP8Type.FLOAT8E4M3FNUZ:
        return value >= 8 and value <= 16
    else:  # E5M2
        return value >= 4 and value <= 8

def write_cell(file, type: FP8Type, byte: int):
    sign = byte >> 7
    if type == FP8Type.FLOAT8E5M2 or type == FP8Type.FLOAT8E5M2FNUZ:
        exponent = (byte >> 2) & 0b11111
        exponent_all_ones = exponent == 0b11111
        exponent_max = 0b11111
        mantissa = byte & 0b11
        mantissa_all_ones = mantissa == 0xb11
    else:  # E4M3.
        exponent = (byte >> 3) & 0b1111
        exponent_all_ones = exponent == 0b1111
        exponent_max = 0b1111
        mantissa = byte & 0b111
        mantissa_all_ones = mantissa == 0b111

    if exponent == 0 and mantissa == 0:
        if type == FP8Type.FLOAT8E4M3FN or type == FP8Type.FLOAT8E5M2:
            file.write(f'<td style="background-color:{BKG_COLOR_ZERO}; font-weight:bold">+0' if sign == 0 else f'<td style="background-color:{BKG_COLOR_ZERO}; font-weight:bold">-0')
        else:  # UZ types.
            file.write(f'<td style="background-color:{BKG_COLOR_ZERO}; font-weight:bold">0' if sign == 0 else f'<td style="background-color:{BKG_COLOR_NAN}; font-weight:bold">NaN')
        return
    elif exponent_all_ones:
        if mantissa == 0:
            if type == FP8Type.FLOAT8E5M2:
                file.write(f'<td style="background-color:{BKG_COLOR_POS_INF}; font-weight:bold">+INF' if sign == 0 else f'<td style="background-color:{BKG_COLOR_NEG_INF}; font-weight:bold">-INF')
                return
        elif mantissa_all_ones:
            if type == FP8Type.FLOAT8E4M3FN or type == FP8Type.FLOAT8E5M2:
                file.write(f'<td style="background-color:{BKG_COLOR_NAN}; font-weight:bold">+NaN' if sign == 0 else f'<td style="background-color:{BKG_COLOR_NAN}; font-weight:bold">-NaN')
                return
        else:
            if type == FP8Type.FLOAT8E5M2:
                file.write(f'<td style="background-color:{BKG_COLOR_NAN}; font-weight:bold">+NaN' if sign == 0 else f'<td style="background-color:{BKG_COLOR_NAN}; font-weight:bold">-NaN')
                return

    class_html = ''
    if exponent != 0:
        if type == FP8Type.FLOAT8E4M3FN:
            value = 2 ** (exponent - 7) * (1 + mantissa * (2 ** -3))
        elif type == FP8Type.FLOAT8E5M2:
            value = 2 ** (exponent - 15) * (1 + mantissa * (2 ** -2))
        elif type == FP8Type.FLOAT8E4M3FNUZ:
            value = 2 ** (exponent - 8) * (1 + mantissa * (2 ** -3))
        else:  # type == FP8Type.FLOAT8E5M2FNUZ
            value = 2 ** (exponent - 16) * (1 + mantissa * (2 ** -2))
        if is_ordinal(type, value):
            class_html = ' class="ordinal"'
    else:
        if type == FP8Type.FLOAT8E4M3FN:
            value = 2 ** (-6) * mantissa * (2 ** -3)
        elif type == FP8Type.FLOAT8E5M2:
            value = 2 ** (-14) * mantissa * (2 ** -2)
        elif type == FP8Type.FLOAT8E4M3FNUZ:
            value = 2 ** (-7) * mantissa * (2 ** -3)
        else:  # type == FP8Type.FLOAT8E5M2FNUZ
            value = 2 ** (-15) * mantissa * (2 ** -2)
        class_html = ' class="denorm"'

    sign_char = '' if sign == 0 else '-'
    formatted_value = format_value(value)
    #bkg_color = '#cfc' if sign == 0 else '#fcc'
    if sign == 0:  # Positive number.
        bkg_color = interpolate_format_color(BKG_COLOR_BASE, BKG_COLOR_MAX_POS, exponent / exponent_max)
    else:  # Negative number.
        bkg_color = interpolate_format_color(BKG_COLOR_BASE, BKG_COLOR_MAX_NEG, exponent / exponent_max)
    file.write(f'<td style="background-color:{bkg_color}"{class_html}>{sign_char}{formatted_value}')

def write_for_type(file, type: FP8Type):
    file.write("<table class=\"fp8\">\n")
    # Header row - hexadecimal.
    file.write("<tr><td>&nbsp;<td>&nbsp;\n")
    for x in range(16):
        text = hex(x)[2:].upper()
        file.write(f'<th>-{text}')
    file.write("\n")
    # Header row - binary.
    file.write("<tr><td>&nbsp;<td>&nbsp;\n")
    for x in range(16):
        text = bin(x)[2:].zfill(4)
        if type == FP8Type.FLOAT8E5M2 or type == FP8Type.FLOAT8E5M2FNUZ:
            file.write(f'<th>-<span style="color:{FONT_COLOR_EXPONENT}">{text[0:2]}</span><span style="color:{FONT_COLOR_MANTISSA}">{text[2:]}</span>')
        else:  # E4M3
            file.write(f'<th>-<span style="color:{FONT_COLOR_EXPONENT}">{text[0]}</span><span style="color:{FONT_COLOR_MANTISSA}">{text[1:]}</span>')
    file.write("\n")
    # Data rows
    for y in range(16):
        text_hex = hex(y)[2:].upper()
        text_bin = bin(y)[2:].zfill(4)
        file.write(f"<tr><th>{text_hex}-<th><span style=\"color:{FONT_COLOR_SIGN}\">{text_bin[0]}</span><span style=\"color:{FONT_COLOR_EXPONENT}\">{text_bin[1:]}</span>-\n")
        for x in range(16):
            write_cell(file, type, x | y << 4)
        file.write("\n")

    file.write("</table>\n")

def main():
    with open('fp8_tables.html', 'w') as file:
        file.write(html_begin)
        file.write("<h1>FLOAT8E4M3FN</h1>\n")
        write_for_type(file, FP8Type.FLOAT8E4M3FN)
        file.write("<h1>FLOAT8E4M3FNUZ</h1>\n")
        write_for_type(file, FP8Type.FLOAT8E4M3FNUZ)
        file.write("<h1>FLOAT8E5M2</h1>\n")
        write_for_type(file, FP8Type.FLOAT8E5M2)
        file.write("<h1>FLOAT8E5M2FNUZ</h1>\n")
        write_for_type(file, FP8Type.FLOAT8E5M2FNUZ)
        file.write("<h1>Legend</h1>\n")
        file.write(LEGEND)
        file.write("<h1>Capabilities and special values</h1>\n")
        file.write(CAPS)
        file.write(html_end)

if __name__ == '__main__':
    main()
