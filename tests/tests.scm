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
eq?                                     ; #<procedure>
(eq? 'a 'a)                             ; #t
(eq? #\a #\a)                           ; #t
(eq? "abcde" "abcde")                   ; #t
(eq? 'a 'b)                             ; #f
(eq? #\a #\newline)                     ; #f
(eq? "this" "that")                     ; #f
(eq? '(a) '(a))                         ; #f
(eq? (+ 1 3) (+ 2 2))                   ; #t
(eq? '() '())                           ; #t
(eq? + +)                               ; #t
(eq? + -)                               ; #f
(eq? #t #t)                             ; #t
(eq? #f #f)                             ; #t
(eq? #t #f)                             ; #f
(eq? (eq? #t #f) #f)                    ; #t
null?                                   ; #<procedure>
(null? '())                             ; #t
(null? '(    ))                         ; #t
(null? '(()))                           ; #f
(null? 'a)                              ; #f
(null? '(1 2 3))                        ; #f
boolean?                                ; #<procedure>
(boolean? #t)                           ; #t
(boolean? #f)                           ; #t
(boolean? '(#t))                        ; #f
(boolean? #\t)                          ; #f
(boolean? (= 1 1))                      ; #t
(boolean? (= 2 1))                      ; #t
(boolean? "a string")                   ; #f
symbol?                                 ; #<procedure>
(symbol? 'a)                            ; #t
(symbol? 1)                             ; #f
(symbol? '(a))                          ; #f
(symbol? '+)                            ; #t
(symbol? '*)                            ; #t
(symbol? '!$%&*/:<>?^_~+-.@)            ; #t
integer?                                ; #<procedure>
(integer? 0)                            ; #t
(integer? 1231132585)                   ; #t
(integer? -9423)                        ; #t
(integer? 0x800)                        ; #t
(integer? #\g)                          ; #f
(integer? #t)                           ; #f
(integer? '(100))                       ; #f
(integer? "not an integer")             ; #f
(integer? (* 13 12))                    ; #t
char?                                   ; #<procedure>
(char? #\r)                             ; #t
(char? (if (char? #\newline) #\space))  ; #t
(char? 'a)                              ; #f
(char? "j")                             ; #f
(char? '(#\a))                          ; #f
(char? 0x10)                            ; #f
string?                                 ; #<procedure>
(string? "hello world")                 ; #t
(string? "with\nnewlines")              ; #t
(string? '(#\a #\space #\l #\i #\s #\t)); #f
(string? 'a-symbol-is-not-a-string)     ; #f
(string? '())                           ; #f
pair?                                   ; #<procedure>
(pair? '(a . b))                        ; #t
(pair? '(a b c))                        ; #t
(pair? '(a))                            ; #t
(pair? '(a . (b . (c . ()))))           ; #t
(pair? 'a)                              ; #f
(pair? '())                             ; #f
(pair? 100)                             ; #f
(pair? "pair")                          ; #f
procedure?                              ; #<procedure>
(procedure? +)                          ; #t
(procedure? '+)                         ; #f
(procedure? "+")                        ; #f
(procedure? procedure?)                 ; #t

