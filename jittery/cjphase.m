function x = cjphase( p, jn, an, fn0 )
% Compute the common part of the phase jitter for
% harmonic partial number p. jn is the jitter gain
% envelope, an is the coherence, or phase attraction
% envelope. fn0 is a filtered noise source, shared by 
% all coherent partials.
% ------------------------------------------------------
% cjphase_p = cjphase( p, jn, an, fn0 )
%
% cjphase_p = phase modulation samples, as many as the
%             length of jn (must be no greater than the
%             length of an)
% p = harmonic number
% jn = jitter gain envelope
% an = coherence, or phase attraction envelope
% fn0 = shared (among coherent partials) filtered noise 
%       vector (also must be at least as long as jn)
%
% O'Donnel:
% cjphase_p(n) = cjphase_p(n-1) + j(n) * p * a(n) * fn_0(n) mod 2pi
%
N = length(jn);
x = zeros(1, N);
x(1) = jn(1) * p * an(1) * fn0(1);
for j = 2:N
	x(j) = x(j-1) + (jn(j) * p * an(j) * fn0(j));
end
x = mod(x, 2*pi);
endfunction 
