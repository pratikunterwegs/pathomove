
library(data.table)
library(ggplot2)
a = fread("items.csv")

b = fread("moves.csv")
hist(b$trait)

ggplot(b, aes(trait, energy))+
  geom_jitter()

ggplot(b[id < 30,])+
  geom_point(data = a,
             aes(x,y), shape = 1)+
  geom_path(aes(x,y, col = factor(id)),
            show.legend = F)+
  geom_point(aes(x,y, col = factor(id)),
            show.legend = F, size = 0.2)+
  coord_cartesian(xlim = c(0, 100),
                  ylim = c(0, 100), expand = F)

# base heading
x = seq(-100, 100, 1)
y = x

heading = atan2(50 - y, 50 - x)

ggplot()+
  geom_tile(
    aes(x = x,
        y = y,
        fill = heading)
  )
