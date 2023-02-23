RPM

Steps to create an RPM/SRPMS.

0. Update; source, contents of spec file, and spec file name to indicate ver and/or release change (we assume 1.0 below)
1. Determine where your; SPECS, RPMS, SRPMS, etc dirs are. The [RpmDir].
2. Make a dir; somewhere to pre-clean the source. The [CleanDir].
3. Make a dir; to clean (indicating ver) ie [CleanDir]/Euphemia-1.0
4. Copy; Classic-SDK, Euphemia-SDK, and Euphemia to [CleanDir]/Euphemia-1.0
5. Copy;  [CleanDir]/Euphemia-1.0/Euphemia/Euphemia-1.0.spec to [RpmDir]/SPECS
6. Copy;  [CleanDir]/Euphemia-1.0/Euphemia/MakeTarBall.sh to [CleanDir]
7. Cd to; [CleanDir]
8. execute MakeTarBall.sh			!!! RUNNING OUTSIDE OF [CleanDir] WOULD BE VERY BAD !!!!
9. Copy; tar-ball to [RpmDir]/SOURCES
10. Cd to; [RpmDir]/SPECS
11. execute "rpmbuild -ba Euphemia-1.0.spec"


