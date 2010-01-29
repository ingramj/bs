;;; bs library procedures

(define number? integer?)

(define (map proc lst)
  (if (null? lst)
    '()
    (cons (proc (car lst))
          (map proc (cdr lst)))))


(define (for-each proc lst)
  (if (null? lst)
    'ok
    (begin
      (proc (car lst))
      (for-each proc (cdr lst)))))


(define (fold-right proc init lst)
  (if (null? lst)
    init
    (proc (car lst)
          (fold-right proc init (cdr lst)))))


(define (fold-left proc init lst)
  (define (iter result rest)
    (if (null? rest)
      result
      (iter (proc result (car rest))
            (cdr rest))))
  (iter init lst))


(define (reverse lst)
  (fold-left (lambda (i j)
               (cons j i))
             '()
             lst))


(define (append l1 l2)
  (fold-right (lambda (i j)
                (cons i j))
              l2
              l1))


(define (not x)
  (if x #f #t))


(define (newline)
  (display #\newline))


(define (call-with-input-file file proc)
  (let ((port (open-input-file file)))
    (let ((result (proc port)))
      (close-input-port port)
      result)))


(define (call-with-output-file file proc)
  (let ((port (open-output-file file)))
    (let ((result (proc port)))
      (close-output-port port)
      result)))


(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))
(define (caaar x) (car (car (car x))))
(define (caadr x) (car (car (cdr x))))
(define (cadar x) (car (cdr (car x))))
(define (caddr x) (car (cdr (cdr x))))
(define (cdaar x) (cdr (car (car x))))
(define (cdadr x) (cdr (car (cdr x))))
(define (cddar x) (cdr (cdr (car x))))
(define (cdddr x) (cdr (cdr (cdr x))))
(define (caaaar x) (car (car (car (car x)))))
(define (caaadr x) (car (car (car (cdr x)))))
(define (caadar x) (car (car (cdr (car x)))))
(define (caaddr x) (car (car (cdr (cdr x)))))
(define (cadaar x) (car (cdr (car (car x)))))
(define (cadadr x) (car (cdr (car (cdr x)))))
(define (caddar x) (car (cdr (cdr (car x)))))
(define (cadddr x) (car (cdr (cdr (cdr x)))))
(define (cdaaar x) (cdr (car (car (car x)))))
(define (cdaadr x) (cdr (car (car (cdr x)))))
(define (cdadar x) (cdr (car (cdr (car x)))))
(define (cdaddr x) (cdr (car (cdr (cdr x)))))
(define (cddaar x) (cdr (cdr (car (car x)))))
(define (cddadr x) (cdr (cdr (car (cdr x)))))
(define (cdddar x) (cdr (cdr (cdr (car x)))))
(define (cddddr x) (cdr (cdr (cdr (cdr x)))))

'stdlib-loaded

