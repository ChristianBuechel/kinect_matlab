function [a] = skeleton_test();

mf   =  figure(1);
clf;
mf1  =  subplot(1,1,1);
set(mf1,'xlim',[1 640],'ylim',[1 480]);

[a b skel] = getskeleton4(1); % initialize kinect
pause(2);
axes(mf1);
Dimage = imagesc(a);
Xc = [skel([4 3 3 5 6 7 8 7 6 5 3 3 9 10 11 12 11 10 9 3 3 2 1 1 13 14 15 16 15 14 13 1 1 17 18 19 20],6)];
Yc = [skel([4 3 3 5 6 7 8 7 6 5 3 3 9 10 11 12 11 10 9 3 3 2 1 1 13 14 15 16 15 14 13 1 1 17 18 19 20],7)];
d = line(Xc, Yc, 'LineWidth',4,'Color',[0 0 .8]);

[a b skel] = getskeleton4(4); % seated mode

%loop
while 1>0
 [a b skel] = getskeleton4(2); % initialize kinect  
 set(Dimage,'CData',a);
 axis(mf1,'image');
 set(mf1,'xlim',[1 640],'ylim',[1 480],'buttondownfcn',@terminate);
 drawnow;
 if skel(1,1) > 0
  set(d,'Xdata',skel([4 3 3 5 6 7 8 7 6 5 3 3 9 10 11 12 11 10 9 3 3 2 1 1 13 14 15 16 15 14 13 1 1 17 18 19 20],6),...
        'Ydata',skel([4 3 3 5 6 7 8 7 6 5 3 3 9 10 11 12 11 10 9 3 3 2 1 1 13 14 15 16 15 14 13 1 1 17 18 19 20],7));
 drawnow;
 end
 
end;

function terminate(gcbo, eventdata, handles);
[a b skel] = getskeleton4(6); % shutdown kinect
disp('hier');
error;


