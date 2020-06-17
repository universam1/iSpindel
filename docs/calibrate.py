#!/usr/bin/env python
# coding=utf-8

import numpy as np
from sklearn.metrics import r2_score

tilts = [24.34,  30.15,  36.758, 44.78,  51.83,  52.93,  57.715]
platos = [0., 4.7, 9.4, 12.9, 16.9, 17.5, 20.9]

tilt2plato = np.poly1d(np.polyfit(tilts, platos, 3))
print("Data points quality: {}%".format(round(100*r2_score(platos, tilt2plato(tilts)))))
print("Test: tilt:{} plato - measured: {} calculated: {}".format(tilts[0], platos[0], round(tilt2plato(tilts[0]),1)))
print("Test: tilt:{} plato - measured: {} calculated: {}".format(tilts[5], platos[5], round(tilt2plato(tilts[5]),1)))
print("Formula:\n{:+}*tilt^3 {:+}*tilt^2 {:+}*tilt {:+}".format(*tilt2plato.coef))
