(use-modules (guix packages)
	     (guix gexp)
	     (guix build-system meson)
	     (guix build-system gnu)
	     (guix licenses)
	     (gnu packages check)
	     (guix download)
	     (gnu packages gcc)
	     (gnu packages commencement)
	     (gnu packages elf)
	     (gnu packages pkg-config)
	     (gnu packages libusb)
	     (gnu packages algebra))

;;; GNU Guix --- Functional package management for GNU
;;; Copyright © 2012, 2013 Ludovic Courtès <ludo@gnu.org>
;;;
;;; This file is part of GNU Guix.
;;;
;;; GNU Guix is free software; you can redistribute it and/or modify it
;;; under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3 of the License, or (at
;;; your option) any later version.
;;;
;;; GNU Guix is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with GNU Guix.  If not, see <http://www.gnu.org/licenses/>.

;; (define-module (gnu packages ncurses)
;;   #:use-module (guix licenses)
;;   #:use-module (guix packages)
;;   #:use-module (guix download)
;;   #:use-module (guix build-system gnu))

(define ncurses
  (let ((patch-makefile-phase
         '(lambda _
            (for-each patch-makefile-SHELL
                      (find-files "." "Makefile.in"))))
        (configure-phase
         '(lambda* (#:key inputs outputs configure-flags
                    #:allow-other-keys)
            (let* ((bash "bash")
                   (out  (assoc-ref outputs "out")))
              ;; (format #t "configure flags: ~s~%" configure-flags)
              (zero? (apply system* bash "./configure"
                            (string-append "SHELL=" bash)
                            (string-append "CONFIG_SHELL=" bash)
                            (string-append "--prefix=" out)
                            configure-flags)))))
        (post-install-phase
         '(lambda* (#:key outputs #:allow-other-keys)
            (let ((out (assoc-ref outputs "out")))
              ;; When building a wide-character (Unicode) build, create backward
              ;; compatibility links from the the "normal" libraries to the
              ;; wide-character libraries (e.g. libncurses.so to libncursesw.so).
              (with-directory-excursion (string-append out "/lib")
                (for-each (lambda (lib)
                            (define libw.a
                              (string-append "lib" lib "w.a"))
                            (define lib.a
                              (string-append "lib" lib ".a"))
                            (define libw.so.x
                              (string-append "lib" lib "w.so.5"))
                            (define lib.so.x
                              (string-append "lib" lib ".so.5"))
                            (define lib.so
                              (string-append "lib" lib ".so"))

                            (when (file-exists? libw.a)
                              (format #t "creating symlinks for `lib~a'~%" lib)
                              (symlink libw.a lib.a)
                              (symlink libw.so.x lib.so.x)
                              (false-if-exception (delete-file lib.so))
                              (call-with-output-file lib.so
                                (lambda (p)
                                  (format p "INPUT (-l~aw)~%" lib)))))
                          '("curses" "ncurses" "form" "panel" "menu")))))))
    (package
        (name "ncurses")
      (version "5.9")
      (source (origin
               (method url-fetch)
               (uri (string-append "mirror://gnu/ncurses/ncurses-"
                                   version ".tar.gz"))
               (sha256
                (base32
                 "0fsn7xis81za62afan0vvm38bvgzg5wfmv1m86flqcj0nj7jjilh"))
               (patches '("ncurses-5.9-gcc-5.patch"))))
      (build-system gnu-build-system)
      (arguments
       `(#:configure-flags
         `("--with-shared" "--without-debug" "--enable-widec"
           "--with-termlib=tinfo"
           ,(string-append "LDFLAGS=-Wl,-rpath=" (assoc-ref %outputs "out") "/lib")
           ;; By default headers land in an `ncursesw' subdir, which is not
           ;; what users expect.
           ,(string-append "--includedir=" (assoc-ref %outputs "out")
                           "/include")

           ;; C++ bindings fail to build on
           ;; `i386-pc-solaris2.11' with GCC 3.4.3:
           ;; <http://bugs.opensolaris.org/bugdatabase/view_bug.do?bug_id=6395191>.
           ,,@(if (string=? (%current-system) "i686-solaris")
                  '("--without-cxx-binding")
                  '()))
         ;; #:make-flags (list
         ;;               (string-append "CPPFLAGS=-P"))
         #:tests? #f                    ; no "check" target
         #:phases (alist-cons-after
                   'install 'post-install ,post-install-phase
                   (alist-cons-before
                    'configure 'patch-makefile-SHELL
                    ,patch-makefile-phase
                    (alist-replace
                     'configure
                     ,configure-phase
                     %standard-phases)))))
      (inputs `(("gcc@5" ,gcc-toolchain-5)))
      (synopsis "Terminal emulation (termcap, terminfo) library")
      (description
       "The Ncurses (new curses) library is a free software emulation of curses
in System V Release 4.0, and more.  It uses Terminfo format, supports pads
and color and multiple highlights and forms characters and function-key
mapping, and has all the other SYSV-curses enhancements over BSD Curses.

The ncurses code was developed under GNU/Linux.  It has been in use for some
time with OpenBSD as the system curses library, and on FreeBSD and NetBSD as
an external package.  It should port easily to any ANSI/POSIX-conforming
UNIX.  It has even been ported to OS/2 Warp!")
      (license x11)
      (home-page "http://www.gnu.org/software/ncurses/"))))

(package
 (name "chai3d-small")
 (version "3.2.0")
 (source (local-file "." "chai3d-small" #:recursive? #t))
 (build-system meson-build-system)
 (arguments
  `(#:tests? #f
    ;; #:validate-runpath? #t
    #:configure-flags
    `(
      ,(string-append "-Dc_link_args=-Wl,-rpath="
                      (assoc-ref %outputs "out") "/lib"))
    #:phases
    (modify-phases %standard-phases
      (delete 'shrink-runpath)
      (add-after 'install 'fix-rpath
        (lambda* (#:key inputs outputs #:allow-other-keys)
          (let* ((outdir (string-append (assoc-ref outputs "out") "/lib"))
                 (libdir (string-append outdir ":"
                                        (assoc-ref inputs "ncurses") "/lib" ":"
                                        (assoc-ref inputs "gcc:lib") "/lib" ":"
                                        (assoc-ref inputs "gcc-toolchain") "/lib" ":"
                                        ;; (assoc-ref inputs "glibc") "/lib" ":"
                                        )))
            (for-each
              (lambda (file)
                (invoke "patchelf" "--set-rpath" libdir
                        (string-append outdir "/" file)))
              '("libHD.so.3.4" "libHD.so" "libPhantomIOLib42.so"))))))))
 (inputs
  `(("libusb" ,libusb)
    ("eigen" ,eigen)))
 (native-inputs
  `(("pkg-config" ,pkg-config)
    ("catch2" ,catch2)
    ("patchelf" ,patchelf)
    ("gcc:lib" ,gcc "lib")
    ("gcc-toolchain" ,gcc-toolchain)))
 (propagated-inputs
  `(("ncurses" ,ncurses)))
 (synopsis "Stripped-down version of chai3d")
 (description "CHAI3D is a powerful cross-platform C++ simulation framework with over 100+ industries and research institutions developing CHAI3D based applications all around the world in segments such as automotive, aerospace, medical, entertainment and industrial robotics.")
 (home-page "https://gitlab.iit.it/dti/weDRAW/chai3d-small")
 (license bsd-3))
