%
% SCR Light Sensor calculations
%

Ndischarge = 200:50:10000;
MHz = 1e6;
uF = 1e-6;
C = .0987*uF;         % C = 0.1u +-
Fclk = 8*MHz;         % Fclk = 8MHz
log_2 = log(2);       % log(Vcc/Vref)
K = 1/(log_2*Fclk*C); %
R = K * Ndischarge;   % number of 8MHz clock periods while C is discharging from Vcc to Vcc/2
r2lux_gl5528 = @(R_kOhm) exp(143/20)./R_kOhm.^(39/20)
Lux = r2lux_gl5528(.001 * R);
plot(Ndischarge, Lux);
