%%% This function obtains the average lambda over exectuion, the hurst
%%% exponent H, and the sigma to make hotspotness fitting. The problem with
%%% lambda and H is that they assume that all times are the same. However,
%%% applications are greatly phase-dependent. Taking the complete execution
%%% may be a bit misleading.
function [lambda, H, a1, sigma1, a2, sigma2] = analyzeAndTrace()

lambda = 0;
H = 0;
a1 = 0; a2 = 0;
sigma1 = 0; sigma2 = 0;

load('Results.mat');


%% Create trace
% Trace_for_rsim: one line per message, first the cycle of injection and
% then the core_id. Injection starts at cycle 0, although that njection may
% appear very well into the program execution.
inj = injected{1};
k=1;
for j=1:length(injected{1})
    nodess(k) = 0;
    k = k+1;
end

for i=2:length(injected)
    inj = [inj injected{i}];
    for j=1:length(injected{i})
        nodess(k) = i-1;
        k = k+1;
    end
end

inj(2,:) = nodess;
inj = sortrows(inj',1);

inj(:,1) = inj(:,1) - inj(1,1);

%fid = fopen('injTimes.txt','w');
fid2 = fopen('trace_for_rsim.txt','w');
for i=1:length(inj)
%    fprintf(fid,'%d ',inj(i));
    fprintf(fid2,'%d %d\n',inj(i,1),inj(i,2));
end
%fclose(fid);
fclose(fid2);

%% analyze hotspotness
% Basically create a vector with the number of injected packets per node
% and then do the bellshaping (see function below)
% for i=1:length(injected)
%     sizes(i) = length(injected{i});
% end
% [a1, sigma1, a2, sigma2, ~] = bellshaping(sizes');
% 
% %% analyze burstiness
% % Basically use the function below to process the array of arrival times.
% H = genhurst(inj(:,1));

%% calculate average injection rate
% number of messages divided by the time.
lambda = length(inj(:,1)) / inj(end,1); 

end


function [a1, c1, a2, c2, error] = bellshaping(aux)

aux2 = sort(aux);

N = size(aux,1);

j = 1;
for i=1:2:N-1
    aux3(j) = aux2(i);
    j = j+1;
end
    
j = N;
for i=2:2:N
    aux3(j) = aux2(i);
    j = j-1;
end

aux4 = aux3/sum(aux3);

x = linspace(0,1,N);

if(N>4)
    [f1,g1] = fit(x.',aux4.','gauss1');
    [f2,g2] = fit(x.',aux4.','gauss2');
    
    if(g1.rsquare > g2.rsquare || f2.a1 < 0 || f2.a2 < 0)
        f = f1; g = g1;
        %plot(f,x,aux4,'x');
        %hold on
        %norm = normpdf(x,0.5,f.c1);
        %plot(x,norm/sum(norm),x,f.a1 * exp(-((x-0.5)/f.c1).^2),'-g');
        %hold off

        a1 = f.a1;
        c1 = f.c1;
        a2 = 0;
        c2 = 0;
    else
        f = f2; g = g2;
        %plot(f,x,aux4,'x');
        %hold on
        %norm = normpdf(x,0.5,f.c1)+normpdf(x,0.5,f.c2);
        %plot(x,norm/sum(norm),x,f.a1 * exp(-((x-0.5)/f.c1).^2) + f.a2 * exp(-((x-0.5)/f.c2).^2),'-g');
        %hold off
    
        a1 = f.a1;
        c1 = f.c1;
        a2 = f.a2;
        c2 = f.a2;
    end
        
else 
    [f,g] = fit(x.',aux4.','gauss1');
    %plot(f,x,aux4,'x');
    %hold on
    %norm = normpdf(x,0.5,f.c1);
    %plot(x,norm/sum(norm),x,f.a1 * exp(-((x-0.5)/f.c1).^2),'-g');
    %hold off

    a1 = f.a1;
    c1 = f.c1;
    a2 = 0;
    c2 = 0;

end

error = g.rsquare;
%annotation('textbox',[0.15 0.65 0.3 0.15],'String',{'R^{2} is ',num2str(g.rsquare)},'LineStyle','none');

end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculates the generalized Hurst exponent H(q) from the scaling 
% of the renormalized q-moments of the distribution 
%
%       <|x(t+r)-x(t)|^q>/<x(t)^q> ~ r^[qH(q)]
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% H = genhurst(S)
% S is 1xT data series (T>50 recommended)
% calculates H(q=1)
%
% H = GenHurst(S,q)
% specifies the exponent q which can be a vector (default value q=1)
%
% H = genhurst(S,q,maxT)
% specifies value maxT of the scaling window, default value maxT=19
%
% [H,sH]=GenHurst(S,...)
% estimates the standard deviation sH(q)
%
% example:
%   generalized Hurst exponent for a random gaussian process
%   H=genhurst(cumsum(randn(10000,1)))
% or 
%   H=genhurst(cumsum(randn(10000,1)),q) to calculate H(q) with arbitrary q
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% for the generalized Hurst exponent method please refer to:
%
%   T. Di Matteo et al. Physica A 324 (2003) 183-188 
%   T. Di Matteo et al. Journal of Banking & Finance 29 (2005) 827-851
%   T. Di Matteo Quantitative Finance, 7 (2007) 21-36
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%    Tomaso Aste   30/01/2013     %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [mH,sH]=genhurst(S,q,maxT) 
if nargin < 2, q = 1; maxT = 19; end
if nargin < 3,  maxT = 19; end
if size(S,1)==1 & size(S,2)>1
    S = S';
elseif size(S,1)>1 & size(S,2)>1
    fprintf('S must be 1xT  \n')
    return
end
if size(S,1) < (maxT*4 | 60)
    warning('Data serie very short!')
end
L=length(S);
lq = length(q);
H  = [];
k = 0;
for Tmax=5:maxT
    k = k+1;
    x = 1:Tmax;
    mcord = zeros(Tmax,lq);
 	for tt = 1:Tmax
        dV = S((tt+1):tt:L) - S(((tt+1):tt:L)-tt);
        VV = S(((tt+1):tt:(L+tt))-tt)';
        N = length(dV)+1;
        X = 1:N;
        Y = VV;
        mx = sum(X)/N;
        SSxx = sum(X.^2) - N*mx^2;
        my   = sum(Y)/N;
        SSxy = sum(X.*Y) - N*mx*my;
        cc(1) = SSxy/SSxx;
        cc(2) = my - cc(1)*mx;
        ddVd  = dV - cc(1);
        VVVd  = VV - cc(1).*(1:N) - cc(2);
         %figure
         %plot(X,Y,'o')
         %hold on
         %plot(X,cc(1)*X+cc(2),'-r')
         %figure
         %plot(1:N-1,dV,'ob')
         %hold on
         %plot([1 N-1],mean(dV)*[1 1],'-b')
         %plot(1:N-1,ddVd,'xr')
         %plot([1 N-1],mean(ddVd)*[1 1],'-r')
        for qq=1:lq
            mcord(tt,qq)=mean(abs(ddVd).^q(qq))/mean(abs(VVVd).^q(qq));
        end
    end
    mx = mean(log10(x));
    SSxx = sum(log10(x).^2) - Tmax*mx^2;
    for qq=1:lq
        my = mean(log10(mcord(:,qq)));
        SSxy = sum(log10(x).*log10(mcord(:,qq))') - Tmax*mx*my;
        H(k,qq) = SSxy/SSxx;
    end
end
%figure
%loglog(x,mcord,'x-')
mH = mean(H)'./q(:);
if nargout == 2
    sH = std(H)'./q(:);
elseif nargout == 1
    sH = [];
end

end

