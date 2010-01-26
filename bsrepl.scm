;; bs REPL
(load "stdlib.scm")

(define (bs-repl)
  (display "bs> ")
  (let ((expr (read (stdin-port))))
    (cond ((eof-object? expr)
           (display "goodbye!\n")
           'ok)
          (else
            (write (eval expr (interaction-environment)))
            (newline)
            (bs-repl)))))

(display "Welcome to the bs REPL. Press ctrl-d to exit.\n")
(bs-repl)

