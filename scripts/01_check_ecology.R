# check function
Rcpp::compileAttributes()
devtools::build()
{sink(file = "install_output.log"); devtools::install(upgrade = "never"); sink()}
devtools::document()

detach(package:snevo)
library(snevo)
library(ggplot2)
library(data.table)

l = snevo::get_test_landscape(
  nItems = 2000,
  landsize = 30,
  nClusters = 30, 
  clusterSpread = 1
)
plot(l)

a = run_pathomove(
  scenario = 1,
  popsize = 200,
  nItems = 2000,
  landsize = 30,
  nClusters = 30,
  clusterSpread = 1,
  tmax = 100,
  genmax = 500,
  range_food = 0.5,
  range_agents = 1,
  handling_time = 5,
  regen_time = 7,
  pTransmit = 0.0005,
  initialInfections = 2,
  costInfect = 0.2
)

names(a)

plot(a[["gens"]], a[["n_infected"]], type = "b")
plot(a[["gens"]], a[["p_src"]], type = "b")


#### handle data ####
b = copy(a)
b = Map(function(l, g) {
    l$id = seq(nrow(l))
    l$gen = g
    l
}, b$pop_data, b$gens)
b = rbindlist(b)
b
# b = b[(gen %% 100 == 0) | (gen == 9999),] 

ggplot(b)+
  geom_bin2d(
    aes(gen, energy)
    # binwidth = c(10, 1)
  )

#### plot data ####
b = melt(b, id.vars = c("gen", "id"))

# energy = b[variable == "energy",]
wts = b[!variable %in% c("energy", "assoc"),]

wts[, value := tanh(value * 20)]

ggplot(wts)+
  geom_hline(
    yintercept = 0,
    col = "blue",
    size = 0.1
  )+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(10, 0.02),
    show.legend = F
  )+
  scale_y_continuous(
    # limits = c(-0.3, 0.3),
    # limits = c(-0.5, 0.5)
    # trans = ggallin::ssqrt_trans
  )+
  scale_fill_viridis_c(
    option = "C", direction = -1,
    begin = 0, end = 1
  )+
  theme_test()+
  facet_wrap(~variable, ncol = 2)

# plot associations per gen
ggplot(b[variable == "assoc"])+
  geom_bin2d(
    aes(gen, value),
    binwidth = c(2, 200)
  )+
  scale_fill_viridis_c(
    option = "H", direction = -1,
    limits = c(3, NA),
    na.value = "transparent",
    trans = "sqrt"
  )

# scale weights
wts_wide = copy(b[!variable %in% c("energy", "assoc"),])
wts_wide[, scaled_val := value / sum(abs(value)),
         by = c("gen", "id")]
wts_wide = dcast(
  wts_wide[, !("value")], 
  gen + id ~ variable, 
  value.var = "scaled_val"
)

ggplot(wts_wide[gen %% 100 == 0 | gen == max(gen)])+
  geom_hline(
    yintercept = 0, col = 2
  )+
  geom_vline(
    xintercept = 0, col = 2
  )+
  geom_jitter(
    aes(coef_nbrs, coef_food2,
        fill = coef_nbrs2),
    shape = 21,
    size = 2
  )+
  scale_fill_viridis_c(
    option = 'H',
    limits = c(-1, 1), 
    direction = -1
  )+
  scale_x_continuous(
    trans = ggallin::ssqrt_trans
  )+
  scale_y_continuous(
    trans = ggallin::ssqrt_trans
  )+
  theme_bw()+
  facet_wrap(~ gen)+
  coord_cartesian(
    xlim = c(-1, 1),
    ylim = c(-1, 1)
  )
