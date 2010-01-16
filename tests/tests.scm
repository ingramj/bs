0                                       ; 0
451                                     ; 451
-219                                    ; -219
0x1f                                    ; 31
021                                     ; 17
#t                                      ; #t
#f                                      ; #f
#\a                                     ; #\a
#\space                                 ; #\space
#\newline                               ; #\newline
"strings"                               ; "strings"
"strings with \"embedded\" quotes"      ; "strings with \"embedded\" quotes"
"strings with\nnewlines"                ; "strings with\nnewlines"
"strings with\\slashes"                 ; "strings with\\slashes"
'()                                     ; ()
'(+ 1 2 3)                              ; (+ 1 2 3)
'(a . b)                                ; (a . b)
'(a . (b . c))                          ; (a b . c)
'(a . (b . (c . ())))                   ; (a b c)
'asdf                                   ; asdf
(define a 100)                          ; ok
a                                       ; 100
(set! a 150)                            ; ok
a                                       ; 150
(define b a)                            ; ok
b                                       ; 150
(set! a 300)                            ; ok
b                                       ; 150

