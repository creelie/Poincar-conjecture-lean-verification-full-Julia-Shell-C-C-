# Ricci flow dg/dt = -2 Ric on the round 3-sphere of radius r: Ric = (2/r^2) g, so
# d(r^2)/dt = -4 and the sphere becomes extinct at T = r0^2/4 (the model for finite-time
# extinction in Perelman's proof). The scalar curvature R = 6/r^2 satisfies R (T - t) = 3/2.
# The neck S^2 x R has d(r^2)/dt = -2, so it pinches at r0^2/2. Integrated with RK4.

function rk4(f, y0, t1; n = 200_000)
    h = t1 / n; y = y0; t = 0.0
    for _ in 1:n
        k1 = f(t, y); k2 = f(t + h/2, y + h/2*k1); k3 = f(t + h/2, y + h/2*k2); k4 = f(t + h, y + h*k3)
        y += h/6*(k1 + 2k2 + 2k3 + k4); t += h
    end
    return y
end

fails = 0
r0 = 1.7
# radius ODE for S^n: dr/dt = -(n-1)/r
for (name, n, T) in (("S^3", 3, r0^2/4), ("neck S^2 x R", 2, r0^2/2))
    t1 = 0.999 * T
    r = rk4((t, r) -> -(n - 1) / r, r0, t1)
    exact = sqrt(r0^2 - 2 * (n - 1) * t1)
    println(name, ": r(0.999 T) = ", r, ", exact ", exact)
    if abs(r - exact) > 1e-8
        global fails += 1
    end
end
for s in (0.5, 0.9, 0.99)
    t = s * r0^2 / 4
    R = 6 / (r0^2 - 4t)
    println("S^3: R (T - t) at t = ", s, " T: ", R * (r0^2/4 - t))
    if abs(R * (r0^2/4 - t) - 1.5) > 1e-12
        global fails += 1
    end
end
println(fails == 0 ? "OK" : "FAILED")
exit(fails == 0 ? 0 : 1)
