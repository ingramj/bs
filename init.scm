;;; bs library procedures

(define (map proc lst)
  (if (null? lst)
    '()
    (cons (proc (car lst))
          (map proc (cdr lst)))))


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

(define (not x)
  (if (eq? x #f)
    #t
    #f))

