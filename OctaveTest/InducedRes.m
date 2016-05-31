%Test function 
clear all


g=9.81; 
rho_a= 1.2; 

LWL=6.096; 

% ----- Verify the linearity Ri/Fh^2 -----------------------------------
% set one heel angle : 20 deg
PHI_= 20*pi/180;

% Loop on the velocity
nVelocities=3
for v=0:1:nVelocities
  
  V_= ( 1./nVelocities * (v+1) ) * sqrt(g * LWL);
  
  % Set the real wind
  twv_=8.38;
  twa_=35*pi/18; 
  
  % Compute the apparent wind  
  awv_=[ V_ + twv_ * cos( twa_ ), twv_ * sin( twa_ ) ]
	awa_= atan2( awv_(1),awv_(2) )

  % Set sail coeffs for small aoa
  cl_=1.6;
  cd_=0.05
  
  % Set the sail nominal area
  An_= 23.6259;
  
  % Compute lift and drag
  lift_ = 0.5 * rho_a * awv_ * awv_ * An_ * cos( PHI_ ) * cl_;
	drag_ = 0.5 * rho_a * awv_ * awv_ * An_ * cos( PHI_ ) * cd_;

  % Updates Fdrive = lift_ * sin(awa) - D * cos(awa);
  fDrive_ = lift_ * sin( awa_ ) - drag_ * cos( awa_ );

	% Updates FSide = L * cos(alfa_eff) + D * sin(alfa_eff);
	fSide_ = lift_ * cos( awa_ ) + drag_ * sin( awa_ );

  % Computes fHeel
  
  fHeel_ = fSide_ 
  
end


