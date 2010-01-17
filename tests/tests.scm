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
(if #t 1 0)                             ; 1
(if #f 1 0)                             ; 0
(if 0 'a 'b)                            ; a
(if 1 'a)                               ; a
(if #f 'a)                              ; #f
+                                       ; #<procedure>
(+ 1 1)                                 ; 2
(+)                                     ; 0
(+ (+ 4 5) 1)                           ; 10
((if #t + -) 8 6)                       ; 14
-                                       ; #<procedure>
(- 1)                                   ; -1
(- 10 50)                               ; -40
(- 0 17)                                ; -17
(- 16 0)                                ; 16
((if #f + -) (if #t 8 18) 10)           ; -2
*                                       ; #<procedure>
(*)                                     ; 1
(* 3)                                   ; 3
(* 3 9)                                 ; 27
(= 0 0)                                 ; #t
(= (+ 1 3) 4)                           ; #t
(= 5 3)                                 ; #f
(if (= (* 2 2) (+ 2 2)) 'a 'b)          ; a
(if (= 6 (- 15 10)) 'a 'b)              ; b
length                                  ; #<procedure>
(length '())                            ; 0
(length '(1))                           ; 1
(length '(a b c))                       ; 3
(length '(+ 1 2 (- 3 4)))               ; 4

