@echo off
chcp 65001 >nul
cd /d D:\code\cpp-job-hunt
echo ================================
echo   cpp-job-hunt  提交并推送
echo ================================
git status --short
echo.
set /p msg="commit message (e.g. feat: xxx): "
if "%msg%"=="" (
    echo 已取消：提交说明不能为空。
    pause
    exit /b 1
)
git add -A
git commit -m "%msg%"
if errorlevel 1 (
    echo.
    echo 提交失败（可能没有改动，或提交说明为空）。
    pause
    exit /b 1
)
git push
echo.
echo ==== 完成 ====
git log --oneline -3
pause
