document.getElementById('themeSelect').addEventListener('change', function(e){
    document.documentElement.setAttribute('data-theme', e.target.value);
    localStorage.setItem('modernui-theme', e.target.value);
});
var saved = localStorage.getItem('modernui-theme');
if (saved) { document.documentElement.setAttribute('data-theme', saved); document.getElementById('themeSelect').value = saved; }
