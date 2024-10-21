pkgload::load_all()

ok <- FALSE

tryCatch(
  fun(),
  interrupt = function(e) {
    message("Interrupted, seen in R code")
    ok <<- TRUE
  }
)

ok
