N = 3000;
b = [0.0001]; a = [1 ,-.9999];
fn0 = filter(b,a,randn(1,N));
fn1 = filter(b,a,randn(1,N));
fn2 = filter(b,a,randn(1,N));
fn3 = filter(b,a,randn(1,N));
fundn = 0.01*pi * ones(1,N);
an = [1:N] / N;
jn = zeros(1,N);
p1 = pphase(1, 0, fundn, jn, an, fn0, fn1 );
p2 = pphase(2, 0, fundn, jn, an, fn0, fn2 );
p3 = pphase(3, 0, fundn, jn, an, fn0, fn3 );
x = cos(p1) + cos(p2) + cos(p3);
jn = 10*ones(1,N);
p1 = pphase(1, 0, fundn, jn, an, fn0, fn1 );
p2 = pphase(2, 0, fundn, jn, an, fn0, fn2 );
p3 = pphase(3, 0, fundn, jn, an, fn0, fn3 );
y = cos(p1) + cos(p2) + cos(p3);
subplot(2,1,1)
plot(x)
subplot(2,1,2)
plot(y)
