;
; thresh-pca.scm
;
; Perform a Thresholding Principal Component Analsysis
;
; Copyright (c) 2017 Linas Vepstas
;
; ---------------------------------------------------------------------
; OVERVIEW
; --------
; A simple, low-brow iterative method to get a quick approximation
; to the first PCA component.  This will be used to create a kind of
; sparse PCA vector, by passing it through a sigmoid function.
; This will in turn be used identify set membership. A detailed
; description is available in the diary.
;
; Write p(x,y) for the probability of the ordered pair, as elsewhere
; throughout this code.  Write P for the index-free matrix; write P^T
; for it's transpose.  Then, given an input vector b, there are two
; kinds of iterations one can perform: a "left" and a "right" iteration.
; The "left iteration" is defined as PP^Tb and the "right iteration" as
; P^TPb.  Note that the dimension of "b" is different for these two cases,
; with dim b = right-dim of P for the right-iteration case.
;
; In index notation, a single "left iteration" step is then the
; computation of the double sum
;
;    s(w) = sum_y p(w,y) sum_x p(x,y) b(x)
;
; and the "right iteration" is
;
;    s(z) = sum_x p(x,z) sum_y p(x,y) b(y)
;
; Because P is sparse, it makes the most sense to compute the inner sum
; "on demand", for only those index values where the outer sum is
; non-vanishing. Here, "on-demand" is what scheme (or functional languages
; in general) excell in: lazy evaluation.  That is, we won't calulate
; anything until you ask for it; and it seems that because P is sparse,
; chances are good you'll never ever ask for it.
;
; So: How should the vectors 'b' and 's' be represented?  As "on demand",
; lazy-evaluation functions.  Give it an argument, and it will return a
; value... after computing it.  We won't compute a value until you ask
; for it.  Thus, most of the functions below just set up other functions
; that would compute a value, if they were ever asked.
;
; In the below, these lazy vectors are called "fvec"'s.
;
; See the FAQ for why heavy-weight numerical calculations are being done
; in scheme instead of C++.
;
; ---------------------------------------------------------------------
;
(use-modules (srfi srfi-1))
(use-modules (ice-9 optargs)) ; for define*-public

; ---------------------------------------------------------------------

(define*-public (make-thresh-pca LLOBJ #:optional
	; Default is to use the pair-freq method
	(get-value 'pair-freq))
"
  make-thresh-pca LLOBJ - Do the thresholding PCA thing.

  Optionally, the name of a method can be supplied, from which the matrix
  values will be fetched.  If not supplied, it defaults to 'pair-freq,
  and so this object can be used with the default pair-freq-api object
  to work with plain-old frequencies.  But you can get fancier if yo wish.
  Using the MI could be interesting, for example: this would result in
  a MaxEnt style computation, instead of a PCA-style computation.
"
	(let ((llobj LLOBJ)
			(star-obj (add-pair-stars LLOBJ))
		)

		; --------------------
		; Return an fvec of items with uniform weighting. This is a
		; unit vector. i.e. its dot-product with itself is 1.0.

		; What this actually returns is a function, that when
		; called with any argument, returns a constant.
		(define (unit-fvec BASIS-SIZE)
			(define weight (/ 1 (sqrt BASIS-SIZE)))

			(lambda (ITEM) weight))

		; Apply equal weighting to all elements of the left-basis
		; This is the starting vector for one step of left-iterate.
		(define (left-init)
			(unit-fvec (star-obj 'left-basis-size)))

		(define (right-init)
			(unit-fvec (star-obj 'right-basis-size)))

		; --------------------
		; Multiply matrix on the left by FVEC.  That is, return the
		; function
		;     result(y) = sum_x p(x,y) FVEC(x)
		; As always, this returns the function `result`. Call this
		; function with an arguement to force the computation to
		; happen.  Note that this is effectively the transpose of P.
		(define (left-mult LEFT-FVEC)
			(lambda (ITEM)
				(fold
					(lambda (PAIR sum)
						(+ sum
							(* (llobj get-value PAIR)
								(LEFT-FVEC (gdr PAIR)))))
					0
					(star-obj 'left-stars ITEM))))

		; Just like above, but returns the function
		;     result(x) = sum_y p(x,y) FVEC(y)
		(define (right-mult RIGHT-FVEC)
			(lambda (ITEM)
				(fold
					(lambda (PAIR sum)
						(+ sum
							(* (llobj get-value PAIR)
								(RIGHT-FVEC (gdr PAIR)))))
					0
					(star-obj 'right-stars ITEM))))

		; --------------------

		(define (left-iter-once FVEC)
			(right-mult (left-mult FVEC))
		)

		; Methods on this class.
		(lambda (message . args)
			(case message
				((left-initial)           (left-init))
				((right-initial)          (right-init))
				((left-mult)              (apply left-mult args))
				((right-mult)             (apply right-mult args))
				((left-iterate)           (apply left-iter-once args))
				(else (apply llobj        (cons message args))))))
)

; ---------------------------------------------------------------------
; ---------------------------------------------------------------------
