function x = pphase( p, theta, fundn, jn, an, fn0, fnp )
% Compute the instantaneous phase of a jittery harmonic
% partial.
% ------------------------------------------------------
% pphase_p = pphase( p, theta, fundn, jn, an, fn0, fnp )
%
% pphase_p = jittery instantaneous phase vector, same
%            length as fundn (must be no greater than the
%            length of jn and an)
% p = harmonic number
% theta = starting phase
% fundn = fundamental frequency envelope, partial frequency
%         is p*fundn, frequency is in radians per sample
%         (sensible range is 0 to pi)
% jn = jitter gain envelope
% an = coherence, or phase attraction envelope
% fn0 = shared (among coherent partials) filtered noise 
%       vector (also must be at least as long as jn)
% fnp = independent (from all other partials) filtered noise 
%       vector (also must be at least as long as jn)
%
% O'Donnell:
% phase_p(n) = dphase_p(n) + cjphase_p(n) + ijphase_p(n) mod 2pi
%
% dphase_p(n) = dphase_p(n-1) + p*f(n) mod 2pi
%             (deterministic/sinusoidal phase)
% 
% see cjphase.m and ijphase.m for computation of the 
% jittery parts of the phase.
%
N = length(fundn);
m = 0:N-1;
dphase_p = theta + m.*(p*fundn);
cjphase_p = cjphase( p, jn, an, fn0 );
ijphase_p = ijphase( p, jn, an, fnp );
x = m2pi( dphase_p + cjphase_p + ijphase_p );
endfunction

