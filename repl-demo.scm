;; REPL demo

(define repl-env (environment))         ; create new env for repl
(eval (eq? (load "stdlib.scm") 'stdlib-loaded) repl-env)


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

(rep-loop)

