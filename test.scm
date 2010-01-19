(define (kons x y)
  (lambda (m) (m x y)))
(define (kar z)
  (z (lambda (p q) p)))
(define (kdr z)
  (z (lambda (p q) q)))

