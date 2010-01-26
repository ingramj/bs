;; REPL demo
(begin
  (load "stdlib.scm")
  (define repl-env (environment))
  (load "stdlib.scm" repl-env)
  (define (rep-loop)
    (display "repl> ")
    (let ((expr (read (stdin-port))))
      (cond ((eof-object? expr)
             (display "goodbye!\n")
             'ok)
            (else
              (write (eval expr repl-env))
              (newline)
              (rep-loop)))))
  (display "REPL Demo\n")
  (rep-loop))

