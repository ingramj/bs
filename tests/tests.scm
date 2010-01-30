(load "../stdlib.scm")                  ; stdlib-loaded
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
'ASDF                                   ; asdf
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
quotient                                ; #<procedure>
(quotient 4 3)                          ; 1
(quotient 3 4)                          ; 0
(quotient -10 4)                        ; -2
(quotient -65 -3)                       ; 21
remainder                               ; #<procedure>
(remainder 4 3)                         ; 1
(remainder 3 4)                         ; 3
(remainder -10 4)                       ; -2
(remainder -65 -3)                      ; -2
(= -4 (+ (* 3 (quotient -4 3)) (remainder -4 3)))  ; #t
(= 0 0)                                 ; #t
(= (+ 1 3) 4)                           ; #t
(= 5 3)                                 ; #f
(if (= (* 2 2) (+ 2 2)) 'a 'b)          ; a
(if (= 6 (- 15 10)) 'a 'b)              ; b
<                                       ; #<procedure>
(< 1 2)                                 ; #t
(< 1 1)                                 ; #f
(< 2 1)                                 ; #f
(< 1 2 3 4)                             ; #t
(< 1 2 4 3)                             ; #f
(< -10 0)                               ; #t
>                                       ; #<procedure>
(> 1 2)                                 ; #f
(> 1 1)                                 ; #f
(> 2 1)                                 ; #t
(> 4 3 2 1)                             ; #t
(> 4 3 1 2)                             ; #f
(> 0 -10)                               ; #t
eq?                                     ; #<procedure>
(eq? 'a 'a)                             ; #t
(eq? 'sdfg 'SDFG)                       ; #t
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
list?                                   ; #<procedure>
(list? 'a)                              ; #f
(list? '(a))                            ; #t
(list? '(1 2 3 4 5 6))                  ; #t
(list? '())                             ; #t
(list? '((a b c) (1 2 3)))              ; #t
(list? '(1 . a))                        ; #f
(list? '(1 . (2 . (3 . ()))))           ; #t
procedure?                              ; #<procedure>
(procedure? +)                          ; #t
(procedure? '+)                         ; #f
(procedure? "+")                        ; #f
(procedure? procedure?)                 ; #t
(and (char? #\n) (integer? 10))         ; #t
(and (char? #\n) #f (integer? 10))      ; #f
(and)                                   ; #t
(or (char? #\n) (integer? 10) #f)       ; #t
(or #f (char? #\n) (integer? 10))       ; #t
(or #f)                                 ; #f
(or)                                    ; #f
cons                                    ; #<procedure>
(cons 'a 'b)                            ; (a . b)
(cons 'a (cons 'b 'c))                  ; (a b . c)
(cons 'a (cons 'b (cons 'c '())))       ; (a b c)
(cons '(a b c) '(d e f))                ; ((a b c) d e f)
(pair? (cons 'a 'b))                    ; #t
car                                     ; #<procedure>
(car '(a . b))                          ; a
(car '(a b c))                          ; a
(car '((a b c) d e f))                  ; (a b c)
(car (cons 'a '(b c d)))                ; a
cdr                                     ; #<procedure>
(cdr '(a . b))                          ; b
(cdr '(a b c))                          ; (b c)
(cdr '((a b c) d e f))                  ; (d e f)
(cdr (cons 'a '(b c d)))                ; (b c d)
(car (cdr '(a b c d e)))                ; b
set-car!                                ; #<procedure>
(set-car! '(a b c) 'h)                  ; ok
(define l '(a b c))                     ; ok
(set-car! l 'h)                         ; ok
(car l)                                 ; h
(define l2 '(1 2 3))                    ; ok
(set-car! l2 '(a b c))                  ; ok
(car l2)                                ; (a b c)
set-cdr!                                ; #<procedure>
(set-cdr! '(a b c) 'h)                  ; ok
(define l '(a b c))                     ; ok
(set-cdr! l 'h)                         ; ok
(cdr l)                                 ; h
(define l2 '(1 2 3))                    ; ok
(set-cdr! l2 '(a b c))                  ; ok
(cdr l2)                                ; (a b c)
length                                  ; #<procedure>
(length '())                            ; 0
(length '(1))                           ; 1
(length '(a b c))                       ; 3
(length '(+ 1 2 (- 3 4)))               ; 4
list                                    ; #<procedure>
(list 1)                                ; (1)
(list 'a 'b 'c)                         ; (a b c)
(list)                                  ; ()
(list (+ 1 2) (+ 3 4) (+ 5 6))          ; (3 7 11)
string-append                           ; #<procedure>
(string-append "asdf")                  ; "asdf"
(string-append "asdf" "ghjk")           ; "asdfghjk"
(string-append (symbol->string 'a) "b") ; "ab"
(string-append "hello" " " "world\n")   ; "hello world\n"
char->integer                           ; #<procedure>
(char->integer #\a)                     ; 97
(char->integer #\newline)               ; 10
integer->char                           ; #<procedure>
(integer->char 122)                     ; #\z
(integer->char 47)                      ; #\/
(integer->char (char->integer #\space)) ; #\space
(char->integer (integer->char 9))       ; 9
number->string                          ; #<procedure>
(number->string 0)                      ; "0"
(number->string 123)                    ; "123"
(number->string -9000)                  ; "-9000"
(number->string 0x20)                   ; "32"
string->number                          ; #<procedure>
(string->number "0")                    ; 0
(string->number "123")                  ; 123
(string->number "-9000")                ; -9000
(string->number "0x20")                 ; 32
(string->number (number->string 408))   ; 408
symbol->string                          ; #<procedure>
(symbol->string 'a)                     ; "a"
(symbol->string '+)                     ; "+"
(symbol->string '$s-to-donuts)          ; "$s-to-donuts"
string->symbol                          ; #<procedure>
(lambda () '())                         ; #<procedure>
(define (empty) '())                    ; ok
empty                                   ; #<procedure>
(empty)                                 ; ()
(define add (lambda (x y) (+ x y)))     ; ok
(add 1 2)                               ; 3
map                                     ; #<procedure>
(define (double n) (* 2 n))             ; ok
(map double '(0 1 2 3))                 ; (0 2 4 6)
(map (lambda (z) (+ 1 z)) '(0 1 2 3))   ; (1 2 3 4)
(define (kons x y) (lambda (m) (m x y))); ok
(define (kar z) (z (lambda (p q) p)))   ; ok
(define (kdr z) (z (lambda (p q) q)))   ; ok
(define a-pair (kons 'a 'b))            ; ok
(kar a-pair)                            ; a
(kdr a-pair)                            ; b
(begin 1)                               ; 1
(begin 1 2 3)                           ; 3
(begin (define alpha 1) alpha)          ; 1
(begin (set! alpha 2) alpha)            ; 2
(begin (define (beta x) (+ 1 x)) (beta alpha))  ; 3
(cond (#f 1) ((eq? 'a 'a) 2) (else 3))  ; 2
(cond ((symbol? 1) #t) (else #f))       ; #f
(cond ((< 10 (* 6 2)) #t) (else #f))    ; #t
(define (type-of x) (cond ((integer? x) 'integer) ((boolean? x) 'boolean) (else 'dont-know)))   ; ok
(type-of 10)                            ; integer
(type-of #t)                            ; boolean
(type-of #\c)                           ; dont-know
(let ((x 1) (y 2)) (+ x y))             ; 3
(define (func x) (let ((y 2)) (* x y))) ; ok
(func 5)                                ; 10
(func 18)                               ; 36
apply                                   ; #<procedure>
(apply + '(1 2 3))                      ; 6
(apply symbol? '(asfd))                 ; #t
(apply (lambda (x) (+ x 1)) '(4))       ; 5
interaction-environment                 ; #<procedure>
null-environment                        ; #<procedure>
environment                             ; #<procedure>
(define env (environment))              ; ok
(eval '(define z 25) env)               ; ok
(eval 'z env)                           ; 25
(append '(0 1 2 3) '(4 5 6 7))          ; (0 1 2 3 4 5 6 7)
(append '() '(1 2 3))                   ; (1 2 3)
(append '(2 4 6) '())                   ; (2 4 6)
(call-with-input-file "./tests.scm" peek-char)  ; #\(
