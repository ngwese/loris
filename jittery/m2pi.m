function y = m2pi(x)
% Wrap phase into the range (-pi,pi].
% -------------------------------------
% y = m2pi(x)
% x = unwrapped phase
% y = wrapped phase
%
y = x+2*pi*round(-x/(2*pi));
endfunction
