;; REPL demo
(load "stdlib.scm")


(define (rep-loop)
  (display "repl> ")
  (let ((expr (read (stdin-port))))
    (cond ((eof-object? expr)
           (display "goodbye!\n")
           'ok)
          (else
            (write (eval expr (interaction-environment)))
            (newline)
            (rep-loop)))))

(rep-loop)

