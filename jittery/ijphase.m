function x = ijphase( p, jn, an, fnp )
% Compute the independent part of the phase jitter for
% harmonic partial number p. jn is the jitter gain
% envelope, an is the coherence, or phase attraction
% envelope. fnp is a filtered noise source, for the
% pth harmonic partial, and is independent of the noise
% source for every other partial.
% ------------------------------------------------------
% ijphase_p = ijphase( p, jn, an, fnp )
%
% ijphase_p = phase modulation samples, as many as the
%             length of jn (must be no greater than the
%             length of an)
% p = harmonic number
% jn = jitter gain envelope
% an = coherence, or phase attraction envelope
% fnp = independent (from all other partials) filtered noise 
%       vector (also must be at least as long as jn)
%
% O'Donnell:
% ijphase_p(n) = 
%   sqrt(1-a(n)2) *( ijphase_p(n-1) + j(n) * p * fn_p(n)) mod 2pi
%
% Applying "jump" to the part of the phase that gets scaled
% according to the attracion/coherence parameter.
%
N = length(jn);
a_inv = sqrt(1 - an.^2);
x = zeros(1, N);
x(1) = a_inv(1) * m2pi( jn(1) * p * fnp(1) );
for j = 2:N
	x(j) = a_inv(j) * m2pi( x(j-1) + (jn(j) * p * fnp(j)));
end
endfunction 
