pkgload::load_all()

tryCatch(
  fun(),
  interrupt = function(e) {
    message("Interrupted, seen in R code")
  }
)
