
library(data.table)
library(ggplot2)
a = fread("items.csv")

b = fread("moves.csv")

ggplot(b[gen == max(gen)])+
  geom_histogram(aes(trait),
                 bins = 10)

# bin trait
b[, traitbin := cut(trait, seq(0, 1, 0.01), include.lowest = T)]

bsummary = b[, list(.N,
                    intake = mean(energy),
                    intakesd = sd(energy)), by = c("gen", "traitbin")]


ggplot(bsummary)+
  geom_tile(aes(gen, traitbin,
                fill = N))+
  scale_fill_viridis_c()

ggplot(bsummary)+
  geom_jitter(aes(traitbin, intake),
              shape = 1,
              size = 0.1)
