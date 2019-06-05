dnl Preprocess with m4 to produce ext.el
;;;
;;; ext.el
;;; 
;;; Add bindings to the C mode keymap that insert various structured comment
;;; templates for the ext system
;;;
;;; This file was generated automatically from ext.m4 -- do not edit
;;;
divert(-1)
changequote({,})

define({tolower},{translit({$1},{ABCDEFGHIJKLMNOPQRSTUVWXYZ},{abcdefghijklmnopqrstuvwxyz})})

define({insertTemplateFunction},{(defun ext-insert-{}tolower({$1}) ()
  "Insert a $1 template."
  (interactive)
  (save-excursion (insert "include(templates/$1.templ)")))}
divert(0))

insertTemplateFunction(CFile)

insertTemplateFunction(CHeaderFile)

insertTemplateFunction(Enum)

insertTemplateFunction(Function)

insertTemplateFunction(Macro)

insertTemplateFunction(Struct)

insertTemplateFunction(Variable)

{
(define-key c-mode-map "\M-1" 'ext-insert-cfile)
(define-key c-mode-map "\M-2" 'ext-insert-cheaderfile)
(define-key c-mode-map "\M-3" 'ext-insert-cheaderfile)
(define-key c-mode-map "\M-4" 'ext-insert-function)
(define-key c-mode-map "\M-5" 'ext-insert-macro)
(define-key c-mode-map "\M-6" 'ext-insert-struct)
(define-key c-mode-map "\M-7" 'ext-insert-enum)
(define-key c-mode-map "\M-8" 'ext-insert-variable)
}

;; Local Variables:
;; mode: lisp
;; End:
