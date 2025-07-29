/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2025~Present chengxuewen.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#ifndef _OCTK_PREPROCESSOR_HPP
#define _OCTK_PREPROCESSOR_HPP

/**
 * @brief Stringify preprocessor macro
 */
#define OCTK_PP_STRINGIFY(text)      OCTK_PP_STRINGIFY_IMPL(text)
#define OCTK_PP_STRINGIFY_IMPL(text) #text

/**
 * @brief Identifier concatenation preprocessor macro
 * @code
 * #define FOO(SYMBOL) foo_ ## SYMBOL
 * #define BAR() bar
 * FOO(bar)    // -> foo_bar
 * FOO(BAR())  // -> foo_BAR()
 *
 * #define FOO1(N) OCTK_PP_CONCAT(foo_, N)
 * FOO1(bar)    // -> foo_bar
 * FOO1(BAR())  // -> foo_bar
 * @endcode
 *
 */
#define OCTK_PP_CONCAT(lhs, rhs)      OCTK_PP_CONCAT_IMPL(lhs, rhs)
#define OCTK_PP_CONCAT_IMPL(lhs, rhs) lhs##rhs

#define OCTK_PP_JOIN(lhs, rhs)   OCTK_PP_JOIN_1(lhs, rhs)
#define OCTK_PP_JOIN_1(lhs, rhs) OCTK_PP_JOIN_2(lhs, rhs)
#define OCTK_PP_JOIN_2(lhs, rhs) lhs##rhs

/**
 * @brief Macro parameters preprocessor macro
 * @code
 * #define FOO(A, B) int foo(A x, B y)
 * #define BAR(A, B) FOO(PP_REMOVE_PARENS(A), PP_REMOVE_PARENS(B))
 * #define BAR(A, B) FOO(PP_REMOVE_PARENS(A), PP_REMOVE_PARENS(B))
 * BAR((bool), (std::pair<int, int>))  // -> int foo(bool x, std::pair<int, int> y)
 * @endcode
 * @warning  __VA_ARGS__ support in C99
 */
#define OCTK_PP_REMOVE_PARENS(T)        OCTK_PP_REMOVE_PARENS_IMPL T
#define OCTK_PP_REMOVE_PARENS_IMPL(...) __VA_ARGS__

/**
 * @brief Special symbols preprocessor macro
 */
#define OCTK_PP_COMMA() ,
#define OCTK_PP_LPAREN() (
#define OCTK_PP_RPAREN() )
#define OCTK_PP_EMPTY()

/**
 * @brief numerical inc dec operation preprocessor macro
 * @code
 * OCTK_PP_INC(1)    // -> 2
 * OCTK_PP_DEC(2)    // -> 1
 * OCTK_PP_INC(256)  // -> OCTK_PP_INC_256 (overflow)
 * OCTK_PP_DEC(0)    // -> OCTK_PP_DEC_0  (underflow)
 * @endcode
 */
#define OCTK_PP_INC(N)  OCTK_PP_CONCAT(OCTK_PP_INC_, N)
#define OCTK_PP_INC_0   1
#define OCTK_PP_INC_1   2
#define OCTK_PP_INC_2   3
#define OCTK_PP_INC_3   4
#define OCTK_PP_INC_4   5
#define OCTK_PP_INC_5   6
#define OCTK_PP_INC_6   7
#define OCTK_PP_INC_7   8
#define OCTK_PP_INC_8   9
#define OCTK_PP_INC_9   10
#define OCTK_PP_INC_10  11
#define OCTK_PP_INC_11  12
#define OCTK_PP_INC_12  13
#define OCTK_PP_INC_13  14
#define OCTK_PP_INC_14  15
#define OCTK_PP_INC_15  16
#define OCTK_PP_INC_16  17
#define OCTK_PP_INC_17  18
#define OCTK_PP_INC_18  19
#define OCTK_PP_INC_19  20
#define OCTK_PP_INC_20  21
#define OCTK_PP_INC_21  22
#define OCTK_PP_INC_22  23
#define OCTK_PP_INC_23  24
#define OCTK_PP_INC_24  25
#define OCTK_PP_INC_25  26
#define OCTK_PP_INC_26  27
#define OCTK_PP_INC_27  28
#define OCTK_PP_INC_28  29
#define OCTK_PP_INC_29  30
#define OCTK_PP_INC_30  31
#define OCTK_PP_INC_31  32
#define OCTK_PP_INC_32  33
#define OCTK_PP_INC_33  34
#define OCTK_PP_INC_34  35
#define OCTK_PP_INC_35  36
#define OCTK_PP_INC_36  37
#define OCTK_PP_INC_37  38
#define OCTK_PP_INC_38  39
#define OCTK_PP_INC_39  40
#define OCTK_PP_INC_40  41
#define OCTK_PP_INC_41  42
#define OCTK_PP_INC_42  43
#define OCTK_PP_INC_43  44
#define OCTK_PP_INC_44  45
#define OCTK_PP_INC_45  46
#define OCTK_PP_INC_46  47
#define OCTK_PP_INC_47  48
#define OCTK_PP_INC_48  49
#define OCTK_PP_INC_49  50
#define OCTK_PP_INC_50  51
#define OCTK_PP_INC_51  52
#define OCTK_PP_INC_52  53
#define OCTK_PP_INC_53  54
#define OCTK_PP_INC_54  55
#define OCTK_PP_INC_55  56
#define OCTK_PP_INC_56  57
#define OCTK_PP_INC_57  58
#define OCTK_PP_INC_58  59
#define OCTK_PP_INC_59  60
#define OCTK_PP_INC_60  61
#define OCTK_PP_INC_61  62
#define OCTK_PP_INC_62  63
#define OCTK_PP_INC_63  64
#define OCTK_PP_INC_64  65
#define OCTK_PP_INC_65  66
#define OCTK_PP_INC_66  67
#define OCTK_PP_INC_67  68
#define OCTK_PP_INC_68  69
#define OCTK_PP_INC_69  70
#define OCTK_PP_INC_70  71
#define OCTK_PP_INC_71  72
#define OCTK_PP_INC_72  73
#define OCTK_PP_INC_73  74
#define OCTK_PP_INC_74  75
#define OCTK_PP_INC_75  76
#define OCTK_PP_INC_76  77
#define OCTK_PP_INC_77  78
#define OCTK_PP_INC_78  79
#define OCTK_PP_INC_79  80
#define OCTK_PP_INC_80  81
#define OCTK_PP_INC_81  82
#define OCTK_PP_INC_82  83
#define OCTK_PP_INC_83  84
#define OCTK_PP_INC_84  85
#define OCTK_PP_INC_85  86
#define OCTK_PP_INC_86  87
#define OCTK_PP_INC_87  88
#define OCTK_PP_INC_88  89
#define OCTK_PP_INC_89  90
#define OCTK_PP_INC_90  91
#define OCTK_PP_INC_91  92
#define OCTK_PP_INC_92  93
#define OCTK_PP_INC_93  94
#define OCTK_PP_INC_94  95
#define OCTK_PP_INC_95  96
#define OCTK_PP_INC_96  97
#define OCTK_PP_INC_97  98
#define OCTK_PP_INC_98  99
#define OCTK_PP_INC_99  100
#define OCTK_PP_INC_100 101
#define OCTK_PP_INC_101 102
#define OCTK_PP_INC_102 103
#define OCTK_PP_INC_103 104
#define OCTK_PP_INC_104 105
#define OCTK_PP_INC_105 106
#define OCTK_PP_INC_106 107
#define OCTK_PP_INC_107 108
#define OCTK_PP_INC_108 109
#define OCTK_PP_INC_109 110
#define OCTK_PP_INC_110 111
#define OCTK_PP_INC_111 112
#define OCTK_PP_INC_112 113
#define OCTK_PP_INC_113 114
#define OCTK_PP_INC_114 115
#define OCTK_PP_INC_115 116
#define OCTK_PP_INC_116 117
#define OCTK_PP_INC_117 118
#define OCTK_PP_INC_118 119
#define OCTK_PP_INC_119 120
#define OCTK_PP_INC_120 121
#define OCTK_PP_INC_121 122
#define OCTK_PP_INC_122 123
#define OCTK_PP_INC_123 124
#define OCTK_PP_INC_124 125
#define OCTK_PP_INC_125 126
#define OCTK_PP_INC_126 127
#define OCTK_PP_INC_127 128
#define OCTK_PP_INC_128 129
#define OCTK_PP_INC_129 130
#define OCTK_PP_INC_130 131
#define OCTK_PP_INC_131 132
#define OCTK_PP_INC_132 133
#define OCTK_PP_INC_133 134
#define OCTK_PP_INC_134 135
#define OCTK_PP_INC_135 136
#define OCTK_PP_INC_136 137
#define OCTK_PP_INC_137 138
#define OCTK_PP_INC_138 139
#define OCTK_PP_INC_139 140
#define OCTK_PP_INC_140 141
#define OCTK_PP_INC_141 142
#define OCTK_PP_INC_142 143
#define OCTK_PP_INC_143 144
#define OCTK_PP_INC_144 145
#define OCTK_PP_INC_145 146
#define OCTK_PP_INC_146 147
#define OCTK_PP_INC_147 148
#define OCTK_PP_INC_148 149
#define OCTK_PP_INC_149 150
#define OCTK_PP_INC_150 151
#define OCTK_PP_INC_151 152
#define OCTK_PP_INC_152 153
#define OCTK_PP_INC_153 154
#define OCTK_PP_INC_154 155
#define OCTK_PP_INC_155 156
#define OCTK_PP_INC_156 157
#define OCTK_PP_INC_157 158
#define OCTK_PP_INC_158 159
#define OCTK_PP_INC_159 160
#define OCTK_PP_INC_160 161
#define OCTK_PP_INC_161 162
#define OCTK_PP_INC_162 163
#define OCTK_PP_INC_163 164
#define OCTK_PP_INC_164 165
#define OCTK_PP_INC_165 166
#define OCTK_PP_INC_166 167
#define OCTK_PP_INC_167 168
#define OCTK_PP_INC_168 169
#define OCTK_PP_INC_169 170
#define OCTK_PP_INC_170 171
#define OCTK_PP_INC_171 172
#define OCTK_PP_INC_172 173
#define OCTK_PP_INC_173 174
#define OCTK_PP_INC_174 175
#define OCTK_PP_INC_175 176
#define OCTK_PP_INC_176 177
#define OCTK_PP_INC_177 178
#define OCTK_PP_INC_178 179
#define OCTK_PP_INC_179 180
#define OCTK_PP_INC_180 181
#define OCTK_PP_INC_181 182
#define OCTK_PP_INC_182 183
#define OCTK_PP_INC_183 184
#define OCTK_PP_INC_184 185
#define OCTK_PP_INC_185 186
#define OCTK_PP_INC_186 187
#define OCTK_PP_INC_187 188
#define OCTK_PP_INC_188 189
#define OCTK_PP_INC_189 190
#define OCTK_PP_INC_190 191
#define OCTK_PP_INC_191 192
#define OCTK_PP_INC_192 193
#define OCTK_PP_INC_193 194
#define OCTK_PP_INC_194 195
#define OCTK_PP_INC_195 196
#define OCTK_PP_INC_196 197
#define OCTK_PP_INC_197 198
#define OCTK_PP_INC_198 199
#define OCTK_PP_INC_199 200
#define OCTK_PP_INC_200 201
#define OCTK_PP_INC_201 202
#define OCTK_PP_INC_202 203
#define OCTK_PP_INC_203 204
#define OCTK_PP_INC_204 205
#define OCTK_PP_INC_205 206
#define OCTK_PP_INC_206 207
#define OCTK_PP_INC_207 208
#define OCTK_PP_INC_208 209
#define OCTK_PP_INC_209 210
#define OCTK_PP_INC_210 211
#define OCTK_PP_INC_211 212
#define OCTK_PP_INC_212 213
#define OCTK_PP_INC_213 214
#define OCTK_PP_INC_214 215
#define OCTK_PP_INC_215 216
#define OCTK_PP_INC_216 217
#define OCTK_PP_INC_217 218
#define OCTK_PP_INC_218 219
#define OCTK_PP_INC_219 220
#define OCTK_PP_INC_220 221
#define OCTK_PP_INC_221 222
#define OCTK_PP_INC_222 223
#define OCTK_PP_INC_223 224
#define OCTK_PP_INC_224 225
#define OCTK_PP_INC_225 226
#define OCTK_PP_INC_226 227
#define OCTK_PP_INC_227 228
#define OCTK_PP_INC_228 229
#define OCTK_PP_INC_229 230
#define OCTK_PP_INC_230 231
#define OCTK_PP_INC_231 232
#define OCTK_PP_INC_232 233
#define OCTK_PP_INC_233 234
#define OCTK_PP_INC_234 235
#define OCTK_PP_INC_235 236
#define OCTK_PP_INC_236 237
#define OCTK_PP_INC_237 238
#define OCTK_PP_INC_238 239
#define OCTK_PP_INC_239 240
#define OCTK_PP_INC_240 241
#define OCTK_PP_INC_241 242
#define OCTK_PP_INC_242 243
#define OCTK_PP_INC_243 244
#define OCTK_PP_INC_244 245
#define OCTK_PP_INC_245 246
#define OCTK_PP_INC_246 247
#define OCTK_PP_INC_247 248
#define OCTK_PP_INC_248 249
#define OCTK_PP_INC_249 250
#define OCTK_PP_INC_250 251
#define OCTK_PP_INC_251 252
#define OCTK_PP_INC_252 253
#define OCTK_PP_INC_253 254
#define OCTK_PP_INC_254 255
#define OCTK_PP_INC_255 256

#define OCTK_PP_DEC(N)  OCTK_PP_CONCAT(OCTK_PP_DEC_, N)
#define OCTK_PP_DEC_256 255
#define OCTK_PP_DEC_255 254
#define OCTK_PP_DEC_254 253
#define OCTK_PP_DEC_253 252
#define OCTK_PP_DEC_252 251
#define OCTK_PP_DEC_251 250
#define OCTK_PP_DEC_250 249
#define OCTK_PP_DEC_249 248
#define OCTK_PP_DEC_248 247
#define OCTK_PP_DEC_247 246
#define OCTK_PP_DEC_246 245
#define OCTK_PP_DEC_245 244
#define OCTK_PP_DEC_244 243
#define OCTK_PP_DEC_243 242
#define OCTK_PP_DEC_242 241
#define OCTK_PP_DEC_241 240
#define OCTK_PP_DEC_240 239
#define OCTK_PP_DEC_239 238
#define OCTK_PP_DEC_238 237
#define OCTK_PP_DEC_237 236
#define OCTK_PP_DEC_236 235
#define OCTK_PP_DEC_235 234
#define OCTK_PP_DEC_234 233
#define OCTK_PP_DEC_233 232
#define OCTK_PP_DEC_232 231
#define OCTK_PP_DEC_231 230
#define OCTK_PP_DEC_230 229
#define OCTK_PP_DEC_229 228
#define OCTK_PP_DEC_228 227
#define OCTK_PP_DEC_227 226
#define OCTK_PP_DEC_226 225
#define OCTK_PP_DEC_225 224
#define OCTK_PP_DEC_224 223
#define OCTK_PP_DEC_223 222
#define OCTK_PP_DEC_222 221
#define OCTK_PP_DEC_221 220
#define OCTK_PP_DEC_220 219
#define OCTK_PP_DEC_219 218
#define OCTK_PP_DEC_218 217
#define OCTK_PP_DEC_217 216
#define OCTK_PP_DEC_216 215
#define OCTK_PP_DEC_215 214
#define OCTK_PP_DEC_214 213
#define OCTK_PP_DEC_213 212
#define OCTK_PP_DEC_212 211
#define OCTK_PP_DEC_211 210
#define OCTK_PP_DEC_210 209
#define OCTK_PP_DEC_209 208
#define OCTK_PP_DEC_208 207
#define OCTK_PP_DEC_207 206
#define OCTK_PP_DEC_206 205
#define OCTK_PP_DEC_205 204
#define OCTK_PP_DEC_204 203
#define OCTK_PP_DEC_203 202
#define OCTK_PP_DEC_202 201
#define OCTK_PP_DEC_201 200
#define OCTK_PP_DEC_200 199
#define OCTK_PP_DEC_199 198
#define OCTK_PP_DEC_198 197
#define OCTK_PP_DEC_197 196
#define OCTK_PP_DEC_196 195
#define OCTK_PP_DEC_195 194
#define OCTK_PP_DEC_194 193
#define OCTK_PP_DEC_193 192
#define OCTK_PP_DEC_192 191
#define OCTK_PP_DEC_191 190
#define OCTK_PP_DEC_190 189
#define OCTK_PP_DEC_189 188
#define OCTK_PP_DEC_188 187
#define OCTK_PP_DEC_187 186
#define OCTK_PP_DEC_186 185
#define OCTK_PP_DEC_185 184
#define OCTK_PP_DEC_184 183
#define OCTK_PP_DEC_183 182
#define OCTK_PP_DEC_182 181
#define OCTK_PP_DEC_181 180
#define OCTK_PP_DEC_180 179
#define OCTK_PP_DEC_179 178
#define OCTK_PP_DEC_178 177
#define OCTK_PP_DEC_177 176
#define OCTK_PP_DEC_176 175
#define OCTK_PP_DEC_175 174
#define OCTK_PP_DEC_174 173
#define OCTK_PP_DEC_173 172
#define OCTK_PP_DEC_172 171
#define OCTK_PP_DEC_171 170
#define OCTK_PP_DEC_170 169
#define OCTK_PP_DEC_169 168
#define OCTK_PP_DEC_168 167
#define OCTK_PP_DEC_167 166
#define OCTK_PP_DEC_166 165
#define OCTK_PP_DEC_165 164
#define OCTK_PP_DEC_164 163
#define OCTK_PP_DEC_163 162
#define OCTK_PP_DEC_162 161
#define OCTK_PP_DEC_161 160
#define OCTK_PP_DEC_160 159
#define OCTK_PP_DEC_159 158
#define OCTK_PP_DEC_158 157
#define OCTK_PP_DEC_157 156
#define OCTK_PP_DEC_156 155
#define OCTK_PP_DEC_155 154
#define OCTK_PP_DEC_154 153
#define OCTK_PP_DEC_153 152
#define OCTK_PP_DEC_152 151
#define OCTK_PP_DEC_151 150
#define OCTK_PP_DEC_150 149
#define OCTK_PP_DEC_149 148
#define OCTK_PP_DEC_148 147
#define OCTK_PP_DEC_147 146
#define OCTK_PP_DEC_146 145
#define OCTK_PP_DEC_145 144
#define OCTK_PP_DEC_144 143
#define OCTK_PP_DEC_143 142
#define OCTK_PP_DEC_142 141
#define OCTK_PP_DEC_141 140
#define OCTK_PP_DEC_140 139
#define OCTK_PP_DEC_139 138
#define OCTK_PP_DEC_138 137
#define OCTK_PP_DEC_137 136
#define OCTK_PP_DEC_136 135
#define OCTK_PP_DEC_135 134
#define OCTK_PP_DEC_134 133
#define OCTK_PP_DEC_133 132
#define OCTK_PP_DEC_132 131
#define OCTK_PP_DEC_131 130
#define OCTK_PP_DEC_130 129
#define OCTK_PP_DEC_129 128
#define OCTK_PP_DEC_128 127
#define OCTK_PP_DEC_127 126
#define OCTK_PP_DEC_126 125
#define OCTK_PP_DEC_125 124
#define OCTK_PP_DEC_124 123
#define OCTK_PP_DEC_123 122
#define OCTK_PP_DEC_122 121
#define OCTK_PP_DEC_121 120
#define OCTK_PP_DEC_120 119
#define OCTK_PP_DEC_119 118
#define OCTK_PP_DEC_118 117
#define OCTK_PP_DEC_117 116
#define OCTK_PP_DEC_116 115
#define OCTK_PP_DEC_115 114
#define OCTK_PP_DEC_114 113
#define OCTK_PP_DEC_113 112
#define OCTK_PP_DEC_112 111
#define OCTK_PP_DEC_111 110
#define OCTK_PP_DEC_110 109
#define OCTK_PP_DEC_109 108
#define OCTK_PP_DEC_108 107
#define OCTK_PP_DEC_107 106
#define OCTK_PP_DEC_106 105
#define OCTK_PP_DEC_105 104
#define OCTK_PP_DEC_104 103
#define OCTK_PP_DEC_103 102
#define OCTK_PP_DEC_102 101
#define OCTK_PP_DEC_101 100
#define OCTK_PP_DEC_100 99
#define OCTK_PP_DEC_99  98
#define OCTK_PP_DEC_98  97
#define OCTK_PP_DEC_97  96
#define OCTK_PP_DEC_96  95
#define OCTK_PP_DEC_95  94
#define OCTK_PP_DEC_94  93
#define OCTK_PP_DEC_93  92
#define OCTK_PP_DEC_92  91
#define OCTK_PP_DEC_91  90
#define OCTK_PP_DEC_90  89
#define OCTK_PP_DEC_89  88
#define OCTK_PP_DEC_88  87
#define OCTK_PP_DEC_87  86
#define OCTK_PP_DEC_86  85
#define OCTK_PP_DEC_85  84
#define OCTK_PP_DEC_84  83
#define OCTK_PP_DEC_83  82
#define OCTK_PP_DEC_82  81
#define OCTK_PP_DEC_81  80
#define OCTK_PP_DEC_80  79
#define OCTK_PP_DEC_79  78
#define OCTK_PP_DEC_78  77
#define OCTK_PP_DEC_77  76
#define OCTK_PP_DEC_76  75
#define OCTK_PP_DEC_75  74
#define OCTK_PP_DEC_74  73
#define OCTK_PP_DEC_73  72
#define OCTK_PP_DEC_72  71
#define OCTK_PP_DEC_71  70
#define OCTK_PP_DEC_70  69
#define OCTK_PP_DEC_69  68
#define OCTK_PP_DEC_68  67
#define OCTK_PP_DEC_67  66
#define OCTK_PP_DEC_66  65
#define OCTK_PP_DEC_65  64
#define OCTK_PP_DEC_64  63
#define OCTK_PP_DEC_63  62
#define OCTK_PP_DEC_62  61
#define OCTK_PP_DEC_61  60
#define OCTK_PP_DEC_60  59
#define OCTK_PP_DEC_59  58
#define OCTK_PP_DEC_58  57
#define OCTK_PP_DEC_57  56
#define OCTK_PP_DEC_56  55
#define OCTK_PP_DEC_55  54
#define OCTK_PP_DEC_54  53
#define OCTK_PP_DEC_53  52
#define OCTK_PP_DEC_52  51
#define OCTK_PP_DEC_51  50
#define OCTK_PP_DEC_50  49
#define OCTK_PP_DEC_49  48
#define OCTK_PP_DEC_48  47
#define OCTK_PP_DEC_47  46
#define OCTK_PP_DEC_46  45
#define OCTK_PP_DEC_45  44
#define OCTK_PP_DEC_44  43
#define OCTK_PP_DEC_43  42
#define OCTK_PP_DEC_42  41
#define OCTK_PP_DEC_41  40
#define OCTK_PP_DEC_40  39
#define OCTK_PP_DEC_39  38
#define OCTK_PP_DEC_38  37
#define OCTK_PP_DEC_37  36
#define OCTK_PP_DEC_36  35
#define OCTK_PP_DEC_35  34
#define OCTK_PP_DEC_34  33
#define OCTK_PP_DEC_33  32
#define OCTK_PP_DEC_32  31
#define OCTK_PP_DEC_31  30
#define OCTK_PP_DEC_30  29
#define OCTK_PP_DEC_29  28
#define OCTK_PP_DEC_28  27
#define OCTK_PP_DEC_27  26
#define OCTK_PP_DEC_26  25
#define OCTK_PP_DEC_25  24
#define OCTK_PP_DEC_24  23
#define OCTK_PP_DEC_23  22
#define OCTK_PP_DEC_22  21
#define OCTK_PP_DEC_21  20
#define OCTK_PP_DEC_20  19
#define OCTK_PP_DEC_19  18
#define OCTK_PP_DEC_18  17
#define OCTK_PP_DEC_17  16
#define OCTK_PP_DEC_16  15
#define OCTK_PP_DEC_15  14
#define OCTK_PP_DEC_14  13
#define OCTK_PP_DEC_13  12
#define OCTK_PP_DEC_12  11
#define OCTK_PP_DEC_11  10
#define OCTK_PP_DEC_10  9
#define OCTK_PP_DEC_9   8
#define OCTK_PP_DEC_8   7
#define OCTK_PP_DEC_7   6
#define OCTK_PP_DEC_6   5
#define OCTK_PP_DEC_5   4
#define OCTK_PP_DEC_4   3
#define OCTK_PP_DEC_3   2
#define OCTK_PP_DEC_2   1
#define OCTK_PP_DEC_1   0

/**
 * @brief numerical sub operation preprocessor macro
 * @code
 * OCTK_PP_SUB(2, 2)  // -> 0
 * OCTK_PP_SUB(2, 1)  // -> 1
 * OCTK_PP_SUB(2, 0)  // -> 2
 * @endcode
 */
#define OCTK_PP_SUB(X, Y) OCTK_PP_GET_TUPLE(0, OCTK_PP_WHILE(OCTK_PP_SUB_P, OCTK_PP_SUB_O, (X, Y)))
#define OCTK_PP_SUB_P(V)  OCTK_PP_GET_TUPLE(1, V)
#define OCTK_PP_SUB_O(V)  (OCTK_PP_DEC(OCTK_PP_GET_TUPLE(0, V)), OCTK_PP_DEC(OCTK_PP_GET_TUPLE(1, V)))

/**
 * @brief Non-negative integer multiplication preprocessor macro
 * @code
 * OCTK_PP_MUL(1, 2)  // -> 2
 * OCTK_PP_MUL(2, 1)  // -> 2
 * OCTK_PP_MUL(2, 0)  // -> 0
 * OCTK_PP_MUL(0, 2)  // -> 0
 * @endcode
 */
#define OCTK_PP_MUL(X, Y) OCTK_PP_GET_TUPLE(0, OCTK_PP_WHILE(OCTK_PP_MUL_P, OCTK_PP_MUL_O, (0, X, Y)))
#define OCTK_PP_MUL_P(V)  OCTK_PP_GET_TUPLE(2, V)
#define OCTK_PP_MUL_O(V)                                                                                               \
    (OCTK_PP_ADD(OCTK_PP_GET_TUPLE(0, V), OCTK_PP_GET_TUPLE(1, V)),                                                    \
     OCTK_PP_GET_TUPLE(1, V),                                                                                          \
     OCTK_PP_DEC(OCTK_PP_GET_TUPLE(2, V)))

/**
 * @brief Numerical comparison preprocessor macro
 * @code
 * OCTK_PP_EQUAL(1, 2)  // -> 0
 * OCTK_PP_EQUAL(1, 1)  // -> 1
 * OCTK_PP_EQUAL(1, 0)  // -> 0
 * @endcode
 */
#define OCTK_PP_CMP(X, Y) OCTK_PP_WHILE(OCTK_PP_CMP_P, OCTK_PP_CMP_O, (X, Y))
#define OCTK_PP_CMP_P(V)  OCTK_PP_AND(OCTK_PP_BOOL(OCTK_PP_GET_TUPLE(0, V)), OCTK_PP_BOOL(OCTK_PP_GET_TUPLE(1, V)))
#define OCTK_PP_CMP_O(V)  (OCTK_PP_DEC(OCTK_PP_GET_TUPLE(0, V)), OCTK_PP_DEC(OCTK_PP_GET_TUPLE(1, V)))

#define OCTK_PP_EQUAL(X, Y)        OCTK_PP_IDENTITY(OCTK_PP_EQUAL_IMPL PP_CMP(X, Y))
#define OCTK_PP_EQUAL_IMPL(RX, RY) OCTK_PP_AND(OCTK_PP_NOT(OCTK_PP_BOOL(RX)), OCTK_PP_NOT(OCTK_PP_BOOL(RY)))

/**
 * @brief Less than comparison preprocessor macro
 * @code
 * OCTK_PP_LESS(0, 1)  // -> 1
 * OCTK_PP_LESS(1, 2)  // -> 1
 * OCTK_PP_LESS(1, 1)  // -> 0
 * OCTK_PP_LESS(2, 1)  // -> 0
 * @endcode
 */
#define OCTK_PP_LESS(X, Y)        OCTK_PP_IDENTITY(OCTK_PP_LESS_IMPL OCTK_PP_CMP(X, Y))
#define OCTK_PP_LESS_IMPL(RX, RY) OCTK_PP_AND(OCTK_PP_NOT(OCTK_PP_BOOL(RX)), OCTK_PP_BOOL(RY))

/**
 * @brief Maximum/minimum value comparison preprocessor macro
 * @code
 * OCTK_PP_MIN(0, 1)  // -> 0
 * OCTK_PP_MIN(1, 1)  // -> 1
 * OCTK_PP_MAX(1, 2)  // -> 2
 * OCTK_PP_MAX(2, 1)  // -> 2
 * @endcode
 */
#define OCTK_PP_MIN(X, Y) OCTK_PP_IF(OCTK_PP_LESS(X, Y), X, Y)
#define OCTK_PP_MAX(X, Y) OCTK_PP_IF(OCTK_PP_LESS(X, Y), Y, X)

/**
 * @brief Non-negative integer division/modulization preprocessor macro
 * @code
 * OCTK_PP_DIV(2, 1), OCTK_PP_MOD(2, 1)  // -> 2, 0
 * OCTK_PP_DIV(1, 1), OCTK_PP_MOD(1, 1)  // -> 1, 0
 * OCTK_PP_DIV(0, 1), OCTK_PP_MOD(0, 1)  // -> 0, 0
 * OCTK_PP_DIV(1, 2), OCTK_PP_MOD(1, 2)  // -> 0, 1
 * @endcode
 */
#define OCTK_PP_DIV_BASE(X, Y) OCTK_PP_WHILE(OCTK_PP_DIV_BASE_P, OCTK_PP_DIV_BASE_O, (0, X, Y))
#define OCTK_PP_DIV_BASE_P(V)  OCTK_PP_NOT(OCTK_PP_LESS(OCTK_PP_GET_TUPLE(1, V), OCTK_PP_GET_TUPLE(2, V))) // X >= Y
#define OCTK_PP_DIV_BASE_O(V)                                                                                          \
    (OCTK_PP_INC(OCTK_PP_GET_TUPLE(0, V)),                                                                             \
     OCTK_PP_SUB(OCTK_PP_GET_TUPLE(1, V), OCTK_PP_GET_TUPLE(2, V)),                                                    \
     OCTK_PP_GET_TUPLE(2, V))

#define OCTK_PP_DIV(X, Y) OCTK_PP_GET_TUPLE(0, OCTK_PP_DIV_BASE(X, Y))
#define OCTK_PP_MOD(X, Y) OCTK_PP_GET_TUPLE(1, OCTK_PP_DIV_BASE(X, Y))

/**
 * @brief Logical operations preprocessor macro
 * @code
 * OCTK_PP_AND(OCTK_PP_NOT(0), 1)  // -> 1
 * OCTK_PP_AND(OCTK_PP_NOT(2), 0)  // -> OCTK_PP_AND_PP_NOT_20
 * @endcode
 */
#define OCTK_PP_NOT(N) OCTK_PP_CONCAT(OCTK_PP_NOT_, N)
#define OCTK_PP_NOT_0  1
#define OCTK_PP_NOT_1  0

#define OCTK_PP_AND(A, B) OCTK_PP_CONCAT(OCTK_PP_AND_, OCTK_PP_CONCAT(A, B))
#define OCTK_PP_AND_00    0
#define OCTK_PP_AND_01    0
#define OCTK_PP_AND_10    0
#define OCTK_PP_AND_11    1

/**
 * @brief Boolean conversion preprocessor macro
 * @code
 * OCTK_PP_AND(OCTK_PP_NOT(OCTK_PP_BOOL(2)), OCTK_PP_BOOL(0))  // -> 0
 * OCTK_PP_NOT(OCTK_PP_BOOL(1000))                   // -> OCTK_PP_NOT_PP_BOOLEAN_1000
 * @endcode
 */
#define OCTK_PP_BOOL(N)   OCTK_PP_CONCAT(OCTK_PP_BOOLEAN_, N)
#define OCTK_PP_BOOLEAN_0 0
#define OCTK_PP_BOOLEAN_1 1
#define OCTK_PP_BOOLEAN_2 1
//...

/**
 * @brief Conditional select preprocessor macro
 * @code
 * #define DEC_SAFE(N) OCTK_PP_IF(N, OCTK_PP_DEC(N), 0)
 *
 * DEC_SAFE(2)  // -> 1
 * DEC_SAFE(1)  // -> 0
 * DEC_SAFE(0)  // -> 0
 * @endcode
 */
#define OCTK_PP_IF(PRED, THEN, ELSE) OCTK_PP_CONCAT(OCTK_PP_IF_, OCTK_PP_BOOL(PRED))(THEN, ELSE)
#define OCTK_PP_IF_1(THEN, ELSE)     THEN
#define OCTK_PP_IF_0(THEN, ELSE)     ELSE

/**
 * @brief Conditional select lazy evaluation preprocessor macro
 * @code
 * OCTK_PP_COMMA_IF(0)  // (empty)
 * OCTK_PP_COMMA_IF(1)  // -> ,
 * OCTK_PP_COMMA_IF(2)  // -> ,
 *
 * #define SURROUND(N) OCTK_PP_IF(N, OCTK_PP_LPAREN, [ OCTK_PP_EMPTY)() \
 *                  N                                 \
 *                  OCTK_PP_IF(N, OCTK_PP_RPAREN, ] OCTK_PP_EMPTY)()
 *
 * SURROUND(0)  // -> [0]
 * SURROUND(1)  // -> (1)
 * SURROUND(2)  // -> (2)
 * @endcode
 */
#define OCTK_PP_COMMA_IF(N) OCTK_PP_IF(N, OCTK_PP_COMMA, OCTK_PP_EMPTY)()

/**
 * @brief Subscript access operations preprocessor macro
 * @code
 * OCTK_PP_GET_N(0, foo, bar)  // -> foo
 * OCTK_PP_GET_N(1, foo, bar)  // -> bar
 *
 * OCTK_PP_GET_TUPLE(0, (foo, bar))  // -> foo
 * OCTK_PP_GET_TUPLE(1, (foo, bar))  // -> bar
 *
 * #define FOO(P, T) OCTK_PP_IF(P, OCTK_PP_GET_TUPLE(1, T), OCTK_PP_GET_TUPLE(0, T))
 *
 * FOO(0, (foo, bar))  // -> foo
 * FOO(1, (foo, bar))  // -> bar
 * FOO(0, (baz))       // -> OCTK_PP_GET_N_1(baz) (too few arguments)
 *
 * #define FOO1(P, T) OCTK_PP_IF(P, OCTK_PP_GET_N_1, OCTK_PP_GET_N_0) T
 *
 * FOO(0, (foo, bar))  // -> foo
 * FOO(1, (foo, bar))  // -> bar
 * FOO(0, (baz))       // -> baz
 * @endcode
 */
#define OCTK_PP_VA_OPT_COMMA(...)                                OCTK_PP_COMMA_IF(OCTK_PP_NOT(OCTK_PP_IS_EMPTY(__VA_ARGS__)))
#define OCTK_PP_GET_N(N, ...)                                    OCTK_PP_CONCAT(OCTK_PP_GET_N_, N)(__VA_ARGS__)
#define OCTK_PP_GET_N_0(_0, ...)                                 _0
#define OCTK_PP_GET_N_1(_0, _1, ...)                             _1
#define OCTK_PP_GET_N_2(_0, _1, _2, ...)                         _2
#define OCTK_PP_GET_N_3(_0, _1, _2, _3, ...)                     _3
#define OCTK_PP_GET_N_4(_0, _1, _2, _3, _4, ...)                 _4
#define OCTK_PP_GET_N_5(_0, _1, _2, _3, _4, _5, ...)             _5
#define OCTK_PP_GET_N_6(_0, _1, _2, _3, _4, _5, _6, ...)         _6
#define OCTK_PP_GET_N_7(_0, _1, _2, _3, _4, _5, _6, _7, ...)     _7
#define OCTK_PP_GET_N_8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define OCTK_PP_GET_TUPLE(N, T)                                  OCTK_PP_GET_N(N, OCTK_PP_REMOVE_PARENS(T))

/**
 * @brief Parameter length calculation preprocessor macro
 * @code
 * OCTK_PP_NARG()                // -> 0
 * OCTK_PP_NARG(foo)             // -> 1
 * OCTK_PP_NARG(foo())           // -> 1
 * OCTK_PP_NARG(())              // -> 1
 * OCTK_PP_NARG(()foo)           // -> 1
 * OCTK_PP_NARG(OCTK_PP_EMPTY)    // -> 1
 * OCTK_PP_NARG(OCTK_PP_COMMA)    // -> 1
 * OCTK_PP_NARG(, )              // -> 2
 * OCTK_PP_NARG(foo, bar)        // -> 2
 * OCTK_PP_NARG(, , , )          // -> 4
 * @endcode
 */
#define OCTK_PP_NARG(...) OCTK_PP_GET_N(8, __VA_ARGS__ OCTK_PP_VA_OPT_COMMA(__VA_ARGS__) 8, 7, 6, 5, 4, 3, 2, 1, 0)

/**
 * @brief Length judgment empty preprocessor macro
 * @code
 * OCTK_PP_IS_EMPTY()                // -> 1
 * OCTK_PP_IS_EMPTY(foo)             // -> 0
 * OCTK_PP_IS_EMPTY(foo())           // -> 0
 * OCTK_PP_IS_EMPTY(())              // -> 0
 * OCTK_PP_IS_EMPTY(()foo)           // -> 0
 * OCTK_PP_IS_EMPTY(OCTK_PP_EMPTY)    // -> 0
 * OCTK_PP_IS_EMPTY(OCTK_PP_COMMA)    // -> 0
 * OCTK_PP_IS_EMPTY(, )              // -> 0
 * OCTK_PP_IS_EMPTY(foo, bar)        // -> 0
 * OCTK_PP_IS_EMPTY(, , , )          // -> 0
 * @endcode
 */
#define OCTK_PP_IS_EMPTY(...)                                                                                          \
    OCTK_PP_AND(                                                                                                       \
        OCTK_PP_AND(OCTK_PP_NOT(OCTK_PP_HAS_COMMA(__VA_ARGS__)), OCTK_PP_NOT(OCTK_PP_HAS_COMMA(__VA_ARGS__()))),       \
        OCTK_PP_AND(OCTK_PP_NOT(OCTK_PP_HAS_COMMA(OCTK_PP_COMMA_V __VA_ARGS__)),                                       \
                    OCTK_PP_HAS_COMMA(OCTK_PP_COMMA_V __VA_ARGS__())))
#define OCTK_PP_HAS_COMMA(...) OCTK_PP_GET_N_8(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 0, 0)
#define OCTK_PP_COMMA_V(...)   ,

/**
 * @brief Parameter traversal access preprocessor macro
 * @code
 * #define DO_EACH(VAR, IDX, CTX) OCTK_PP_COMMA_IF(IDX) CTX VAR
 *
 * OCTK_PP_FOR_EACH(DO_EACH, void, )        // (empty)
 * OCTK_PP_FOR_EACH(DO_EACH, int, a, b, c)  // -> int a, int b, int c
 * OCTK_PP_FOR_EACH(DO_EACH, bool, x)       // -> bool x
 * @endcode
 */
#define OCTK_PP_FOR_EACH(DO, CTX, ...)                                                                                 \
    OCTK_PP_CONCAT(OCTK_PP_FOR_EACH_, OCTK_PP_NARG(__VA_ARGS__))(DO, CTX, 0, __VA_ARGS__)
#define OCTK_PP_FOR_EACH_0(DO, CTX, IDX, ...)
#define OCTK_PP_FOR_EACH_1(DO, CTX, IDX, VAR, ...) DO(VAR, IDX, CTX)
#define OCTK_PP_FOR_EACH_2(DO, CTX, IDX, VAR, ...)                                                                     \
    DO(VAR, IDX, CTX)                                                                                                  \
    OCTK_PP_FOR_EACH_1(DO, CTX, OCTK_PP_INC(IDX), __VA_ARGS__)
#define OCTK_PP_FOR_EACH_3(DO, CTX, IDX, VAR, ...)                                                                     \
    DO(VAR, IDX, CTX)                                                                                                  \
    OCTK_PP_FOR_EACH_2(DO, CTX, OCTK_PP_INC(IDX), __VA_ARGS__)
// ...

/**
 * @brief Symbol matching preprocessor macro
 * @code
 * #define IS_VOID_void
 * OCTK_PP_IS_SYMBOL(IS_VOID_, void)            // -> 1
 * OCTK_PP_IS_SYMBOL(IS_VOID_, )                // -> 0
 * OCTK_PP_IS_SYMBOL(IS_VOID_, int)             // -> 0
 * OCTK_PP_IS_SYMBOL(IS_VOID_, void*)           // -> 0
 * OCTK_PP_IS_SYMBOL(IS_VOID_, void x)          // -> 0
 * OCTK_PP_IS_SYMBOL(IS_VOID_, void(int, int))  // -> 0
 *
 * OCTK_PP_IS_PARENS()                // -> 0
 * OCTK_PP_IS_PARENS(foo)             // -> 0
 * OCTK_PP_IS_PARENS(foo())           // -> 0
 * OCTK_PP_IS_PARENS(()foo)           // -> 0
 * OCTK_PP_IS_PARENS(())              // -> 1
 * OCTK_PP_IS_PARENS((foo))           // -> 1
 * OCTK_PP_IS_PARENS(((), foo, bar))  // -> 1
 *
 *
 * #define FOO(A, B) int foo(A x, B y)
 * #define BAR(A, B) FOO(OCTK_PP_TRY_REMOVE_PARENS(A), OCTK_PP_TRY_REMOVE_PARENS(B))
 *
 * FOO(bool, IntPair)                // -> int foo(bool x, IntPair y)
 * BAR(bool, IntPair)                // -> int foo(bool x, IntPair y)
 * BAR(bool, (std::pair<int, int>))  // -> int foo(bool x, std::pair<int, int> y)
 * @endcode
 */
#define OCTK_PP_IS_SYMBOL(PREFIX, SYMBOL) OCTK_PP_IS_EMPTY(OCTK_PP_CONCAT(PREFIX, SYMBOL))
#define OCTK_PP_IS_PARENS(SYMBOL)         OCTK_PP_IS_EMPTY(OCTK_PP_EMPTY_V SYMBOL)
#define OCTK_PP_EMPTY_V(...)
#define OCTK_PP_IDENTITY(N)          N
#define OCTK_PP_TRY_REMOVE_PARENS(T) OCTK_PP_IF(OCTK_PP_IS_PARENS(T), OCTK_PP_REMOVE_PARENS, OCTK_PP_IDENTITY)(T)

/**
 * @brief Recursive Reentry preprocessor macro
 * @code
 * // -> OCTK_PP_FOR_EACH(OCTK_PP_DO_EACH_2, obj.x, x1, x2) OCTK_PP_FOR_EACH(OCTK_PP_DO_EACH_2, obj.y, y1)
 * OCTK_PP_OUTER(obj, ((x, (x1, x2)), (y, (y1))))
 * @endcode
 */
#define OCTK_PP_OUTER(N, T) OCTK_PP_FOR_EACH(OCTK_PP_DO_EACH_1, N, OCTK_PP_REMOVE_PARENS(T))
#define OCTK_PP_DO_EACH_1(VAR, IDX, CTX)                                                                               \
    OCTK_PP_FOR_EACH(OCTK_PP_DO_EACH_2, CTX.OCTK_PP_GET_TUPLE(0, VAR), OCTK_PP_REMOVE_PARENS(OCTK_PP_GET_TUPLE(1, VAR)))
#define OCTK_PP_DO_EACH_2(VAR, IDX, CTX) CTX.VAR = VAR;

/**
 * @brief Conditional Loop preprocessor macro
 * @code
 * #define OP_1(VAL) \
 *   (OCTK_PP_GET_TUPLE(0, OCTK_PP_WHILE(PRED, OP_2, \
 *                             (OCTK_PP_GET_TUPLE(0, VAL), OCTK_PP_GET_TUPLE(1, VAL), \
 *                              OCTK_PP_GET_TUPLE(1, VAL)))), \
 *    OCTK_PP_DEC(OCTK_PP_GET_TUPLE(1, VAL)))
 * #define OP_2(VAL) \
 *   (OCTK_PP_GET_TUPLE(0, VAL) + OCTK_PP_GET_TUPLE(2, VAL) * OCTK_PP_GET_TUPLE(1, VAL), \
 *    OCTK_PP_DEC(PP_GET_TUPLE(1, VAL)), OCTK_PP_GET_TUPLE(2, VAL))
 *
 * OCTK_PP_GET_TUPLE(0, OCTK_PP_WHILE(PRED, OP_1, (x, 2)))  // -> x + 2 * 2 + 2 * 1 + 1 * 1
 * @endcode
 */
#define OCTK_PP_WHILE OCTK_PP_CONCAT(OCTK_PP_WHILE_, OCTK_PP_AUTO_DIM(OCTK_PP_WHILE_CHECK))

#define OCTK_PP_AUTO_DIM(CHECK)    OCTK_PP_IF(CHECK(2), OCTK_PP_AUTO_DIM_12, OCTK_PP_AUTO_DIM_34)(CHECK)
#define OCTK_PP_AUTO_DIM_12(CHECK) OCTK_PP_IF(CHECK(1), 1, 2)
#define OCTK_PP_AUTO_DIM_34(CHECK) OCTK_PP_IF(CHECK(3), 3, 4)

#define OCTK_PP_WHILE_CHECK(N)                        OCTK_PP_CONCAT(OCTK_PP_WHILE_CHECK_, OCTK_PP_WHILE_##N(0 OCTK_PP_EMPTY_V, , 1))
#define OCTK_PP_WHILE_CHECK_1                         1
#define OCTK_PP_WHILE_CHECK_PP_WHILE_1(PRED, OP, VAL) 0
#define OCTK_PP_WHILE_CHECK_PP_WHILE_2(PRED, OP, VAL) 0
#define OCTK_PP_WHILE_CHECK_PP_WHILE_3(PRED, OP, VAL) 0
#define OCTK_PP_WHILE_CHECK_PP_WHILE_4(PRED, OP, VAL) 0
// ...

#endif // _OCTK_PREPROCESSOR_HPP
