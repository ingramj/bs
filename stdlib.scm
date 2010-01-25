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


'stdlib-loaded

