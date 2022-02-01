#region: Spain (Espagna)
#format: <UNICODE>, <ROW><COL>, <MODIFIERS>
# <UNICODE>: hexadecimal unicode number or DEADKEY<N>
# <ROW>: row in keyboard matrix (hexadecimal: 0-B)
# <COL>: column in keyboard matrix (0-7)
# <MODIFIERS>: space separated list of modifiers:
#              SHIFT CTRL GRAPH CODE
#
# Example characters in comments are encoded in UTF-8
#
DEADKEY1, 25,
00000, 02, CTRL SHIFT  # ^@
00001, 26, CTRL        # ^A
00002, 27, CTRL        # ^B
00003, 30, CTRL        # ^C
00004, 31, CTRL        # ^D
00005, 32, CTRL        # ^E
00006, 33, CTRL        # ^F
00007, 34, CTRL        # ^G
00008, 75,             # Backspace
00009, 73,             # Tab
0000a, 37, CTRL        # ^J
0000b, 81,             # Home (is Home a unicode character?)
0000c, 41, CTRL        # ^L
0000d, 77,             # Enter/CR
0000e, 43, CTRL        # ^N
0000f, 44, CTRL        # ^O
00010, 45, CTRL        # ^P
00011, 46, CTRL        # ^Q
00012, 82,             # Insert (is Insert a unicode character?)
00013, 50, CTRL        # ^S
00014, 51, CTRL        # ^T
00015, 52, CTRL        # ^U
00016, 53, CTRL        # ^V
00017, 54, CTRL        # ^W
00018, 76,             # Select (is Select a unicode character?)
00019, 56, CTRL        # ^Y
0001a, 57, CTRL        # ^Z
0001b, 72,             # Escape(SDL maps ESC and ^[ to this code)
0001c, 87,             # Right (SDL maps ^\ to this code)
0001d, 84,             # Left  (SDL maps ^] to this code)
0001e, 85,             # Up    (SDL maps ^^ to this code)
0001f, 86,             # Down  (SDL maps ^/ to this code)
00020, 80,             # Space
00021, 01, SHIFT       # !
00022, 20, SHIFT       # "
00023, 03, SHIFT       # #
00024, 04, SHIFT       # $
00025, 05, SHIFT       # %
00026, 07, SHIFT       # &
00027, 20,             # '
00028, 11, SHIFT       # (
00029, 00, SHIFT       # )
0002a, 10, SHIFT       # *
0002b, 13, SHIFT       # +
0002c, 22,             # ,
0002d, 12,             # -
0002e, 23,             # .
0002f, 24,             # /
00030, 00,             # 0
00031, 01,             # 1
00032, 02,             # 2
00033, 03,             # 3
00034, 04,             # 4
00035, 05,             # 5
00036, 06,             # 6
00037, 07,             # 7
00038, 10,             # 8
00039, 11,             # 9
0003a, 21, SHIFT       # :
0003b, 21,             # ;
0003c, 22, SHIFT       # <
0003d, 13,             # =
0003e, 23, SHIFT       # >
0003f, 24, SHIFT       # ?
00040, 02, SHIFT       # @
00041, 26, SHIFT       # A
00042, 27, SHIFT       # B
00043, 30, SHIFT       # C
00044, 31, SHIFT       # D
00045, 32, SHIFT       # E
00046, 33, SHIFT       # F
00047, 34, SHIFT       # G
00048, 35, SHIFT       # H
00049, 36, SHIFT       # I
0004a, 37, SHIFT       # J
0004b, 40, SHIFT       # K
0004c, 41, SHIFT       # L
0004d, 42, SHIFT       # M
0004e, 43, SHIFT       # N
0004f, 44, SHIFT       # O
00050, 45, SHIFT       # P
00051, 46, SHIFT       # Q
00052, 47, SHIFT       # R
00053, 50, SHIFT       # S
00054, 51, SHIFT       # T
00055, 52, SHIFT       # U
00056, 53, SHIFT       # V
00057, 54, SHIFT       # W
00058, 55, SHIFT       # X
00059, 56, SHIFT       # Y
0005a, 57, SHIFT       # Z
0005b, 15,             # [
0005c, 14,             # \
0005d, 16,             # ]
0005e, 06, SHIFT       # ^
0005f, 12, SHIFT       # _
00060, 43, CODE        # `
00061, 26,             # a
00062, 27,             # b
00063, 30,             # c
00064, 31,             # d
00065, 32,             # e
00066, 33,             # f
00067, 34,             # g
00068, 35,             # h
00069, 36,             # i
0006a, 37,             # j
0006b, 40,             # k
0006c, 41,             # l
0006d, 42,             # m
0006e, 43,             # n
0006f, 44,             # o
00070, 45,             # p
00071, 46,             # q
00072, 47,             # r
00073, 50,             # s
00074, 51,             # t
00075, 52,             # u
00076, 53,             # v
00077, 54,             # w
00078, 55,             # x
00079, 56,             # y
0007a, 57,             # z
0007b, 15, SHIFT       # {
0007c, 14, SHIFT       # |
0007d, 16, SHIFT       # }
0007e, 43, SHIFT CODE  # ~
0007f, 83,             # Delete
000a0, 80,             # No-break space (&nbsp;)
000a1, 01, SHIFT CODE  # ¡
000a2, 04, CODE        # ¢
000a3, 04, SHIFT CODE  # £
000a5, 05, SHIFT CODE  # ¥
000a7, 03, CODE        # §
000aa, 23, CODE        # ª
000ab, 22, SHIFT GRAPH # «
000ac, 56, SHIFT GRAPH # ¬
000b0, 57, SHIFT GRAPH # °
000b1, 13, GRAPH       # ±
000b2, 02, SHIFT GRAPH # ²
000b5, 42, CODE        # µ
000b6, 03, SHIFT CODE  # ¶
000b7, 30, SHIFT GRAPH # ·
000ba, 24, CODE        # º
000bb, 23, SHIFT GRAPH # »
000bc, 01, GRAPH       # ¼
000bd, 02, GRAPH       # ½
000be, 03, GRAPH       # ¾
000bf, 24, SHIFT CODE  # ¿
000c3, 35, SHIFT CODE  # Ã
000c4, 26, SHIFT CODE  # Ä
000c5, 22, SHIFT CODE  # Å
000c6, 37, SHIFT CODE  # Æ
000c7, 11, SHIFT CODE  # Ç
000c9, 52, SHIFT CODE  # É
000d1, 17, SHIFT       # Ñ
000d5, 41, SHIFT CODE  # Õ
000d6, 33, SHIFT CODE  # Ö
000dc, 34, SHIFT CODE  # Ü
000df, 07, CODE        # ß
000e0, 57, CODE        # à
000e1, 56, CODE        # á
000e2, 46, CODE        # â
000e3, 35, CODE        # ã
000e4, 26, CODE        # ä
000e5, 22, CODE        # å
000e6, 37, CODE        # æ
000e7, 11, CODE        # ç
000e8, 55, CODE        # è
000e9, 52, CODE        # é
000ea, 54, CODE        # ê
000eb, 50, CODE        # ë
000ec, 30, CODE        # ì
000ed, 36, CODE        # í
000ee, 32, CODE        # î
000ef, 31, CODE        # ï
000f1, 17,             # ñ
000f2, 53, CODE        # ò
000f3, 44, CODE        # ó
000f4, 47, CODE        # ô
000f5, 41, CODE        # õ
000f6, 33, CODE        # ö
000f7, 24, SHIFT GRAPH # ÷
000f9, 27, CODE        # ù
000fa, 45, CODE        # ú
000fb, 51, CODE        # û
000fc, 34, CODE        # ü
000ff, 05, CODE        # ÿ
00128, 40, SHIFT CODE  # Ĩ
00129, 40, CODE        # ĩ
00132, 20, SHIFT CODE  # Ĳ
00133, 20, CODE        # ĳ
00168, 17, SHIFT CODE  # Ũ
00169, 17, CODE        # ũ
00192, 01, CODE        # ƒ
00393, 10, SHIFT CODE  # Γ
00394, 00, SHIFT CODE  # Δ
00398, 13, CODE        # Θ
003a3, 21, SHIFT CODE  # Σ
003a6, 15, SHIFT CODE  # Φ
003a9, 16, SHIFT CODE  # Ω
003b1, 06, CODE        # α
003b4, 00, CODE        # δ
003c0, 45, SHIFT CODE  # π
003c3, 21, CODE        # σ
003c4, 10, CODE        # τ
003c9, 16, CODE        # ω
02021, 02, CODE        # ‡
02022, 11, GRAPH       # •
02030, 05, GRAPH       # ‰
0207f, 03, SHIFT GRAPH # ⁿ
020a7, 02, SHIFT CODE  # ₧
02205, 15, CODE        # ∅
02208, 12, CODE        # ∈
02219, 55, SHIFT GRAPH # ∙
0221a, 07, GRAPH       # √
0221e, 10, GRAPH       # ∞
02229, 04, GRAPH       # ∩
0223d, 21, GRAPH       # ∽
02248, 21, SHIFT GRAPH # ≈
02261, 13, SHIFT GRAPH # ≡
02264, 22, GRAPH       # ≤
02265, 23, GRAPH       # ≥
02302, --,             # ⌂
02310, 47, SHIFT GRAPH # ⌐
02320, 06, GRAPH       # ⌠
02321, 06, SHIFT GRAPH # ⌡
02500, 12, GRAPH       # ─
02502, 14, SHIFT GRAPH # │
0250c, 47, GRAPH       # ┌
02510, 56, GRAPH       # ┐
02514, 53, GRAPH       # └
02518, 43, GRAPH       # ┘
0251c, 33, GRAPH       # ├
02524, 35, GRAPH       # ┤
0252c, 51, GRAPH       # ┬
02534, 27, GRAPH       # ┴
0253c, 34, GRAPH       # ┼
02571, 24, GRAPH       # ╱
02572, 14, GRAPH       # ╲
02573, 55, GRAPH       # ╳
02580, 36, SHIFT GRAPH # ▀
02582, 52, GRAPH       # ▂
02584, 36, GRAPH       # ▄
02586, 44, GRAPH       # ▆
02588, 45, GRAPH       # █
0258a, 41, GRAPH       # ▊
0258c, 40, GRAPH       # ▌
0258e, 37, GRAPH       # ▎
02590, 40, SHIFT GRAPH # ▐
02596, 35, SHIFT GRAPH # ▖
02597, 33, SHIFT GRAPH # ▗
02598, 43, SHIFT GRAPH # ▘
0259a, 31, SHIFT GRAPH # ▚
0259d, 53, SHIFT GRAPH # ▝
0259e, 31, GRAPH       # ▞
025a0, 26, SHIFT GRAPH # ■
025ac, 26, GRAPH       # ▬
025c7, 30, GRAPH       # ◇
025cb, 00, GRAPH       # ○
025d8, 11, SHIFT GRAPH # ◘
025d9, 00, SHIFT GRAPH # ◙
0263a, 15, GRAPH       # ☺
0263b, 15, SHIFT GRAPH # ☻
0263c, 57, GRAPH       # ☼
02640, 42, SHIFT GRAPH # ♀
02642, 42, GRAPH       # ♂
02660, 17, GRAPH       # ♠
02663, 20, GRAPH       # ♣
02665, 20, SHIFT GRAPH # ♥
02666, 17, SHIFT GRAPH # ♦
0266a, 16, GRAPH       # ♪
0266b, 16, SHIFT GRAPH # ♫
027ca, 34, SHIFT GRAPH # ⟊
1fb6c, 54, GRAPH       # LEFT TRIANGULAR ONE QUARTER BLOCK
1fb6d, 32, GRAPH       # UPPER TRIANGULAR ONE QUARTER BLOCK
1fb6e, 54, SHIFT GRAPH # RIGHT TRIANGULAR ONE QUARTER BLOCK
1fb6f, 32, SHIFT GRAPH # LOWER TRIANGULAR ONE QUARTER BLOCK
1fb82, 44, SHIFT GRAPH # UPPER ONE QUARTER BLOCK
1fb85, 52, SHIFT GRAPH # UPPER THREE QUARTERS BLOCK
1fb87, 41, SHIFT GRAPH # RIGHT ONE QUARTER BLOCK
1fb8a, 37, SHIFT GRAPH # RIGHT THREE QUARTERS BLOCK
1fb96, 45, SHIFT GRAPH # INVERSE CHECKER BOARD FILL
1fb98, 46, GRAPH       # UPPER LEFT TO LOWER RIGHT FILL
1fb99, 46, SHIFT GRAPH # UPPER RIGHT TO LOWER LEFT FILL
1fb9a, 50, SHIFT GRAPH # UPPER AND LOWER TRIANGULAR HALF BLOCK
1fb9b, 50, GRAPH       # LEFT AND RIGHT TRIANGULAR HALF BLOCK
1fbaf, 12, SHIFT GRAPH # BOX DRAWINGS LIGHT HORIZONTAL WITH VERTICAL STROKE
