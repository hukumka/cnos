if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
imap <F3> *
imap <F2> /
nmap  :NERDTreeToggle
nmap d :cs find d =expand("<cword>")	
nmap i :cs find i ^=expand("<cfile>")$
nmap f :cs find f =expand("<cfile>")	
nmap e :cs find e =expand("<cword>")	
nmap t :cs find t =expand("<cword>")	
nmap c :cs find c =expand("<cword>")	
nmap g :cs find g =expand("<cword>")	
nmap s :cs find s =expand("<cword>")	
nmap T :TlistToggle
imap № #
nmap gx <Plug>NetrwBrowseX
nmap j gj
nmap k gk
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
nmap <Nul><Nul>d :vert scs find d =expand("<cword>")
nmap <Nul><Nul>i :vert scs find i ^=expand("<cfile>")$	
nmap <Nul><Nul>f :vert scs find f =expand("<cfile>")	
nmap <Nul><Nul>e :vert scs find e =expand("<cword>")
nmap <Nul><Nul>t :vert scs find t =expand("<cword>")
nmap <Nul><Nul>c :vert scs find c =expand("<cword>")
nmap <Nul><Nul>g :vert scs find g =expand("<cword>")
nmap <Nul><Nul>s :vert scs find s =expand("<cword>")
nmap <Nul>d :scs find d =expand("<cword>")	
nmap <Nul>i :scs find i ^=expand("<cfile>")$	
nmap <Nul>f :scs find f =expand("<cfile>")	
nmap <Nul>e :scs find e =expand("<cword>")	
nmap <Nul>t :scs find t =expand("<cword>")	
nmap <Nul>c :scs find c =expand("<cword>")	
nmap <Nul>g :scs find g =expand("<cword>")	
nmap <Nul>s :scs find s =expand("<cword>")	
map <F10> :make run
map <C-F9> :make debug
map <F9> :make current=%
nmap <F2> :w
map <silent> <F4> :s/^\(\t*\)\(\/\*\)*\(.\{-\}\)\(\*\/\)*$/\1\3/ "pазкомментирование строки
map <silent> <F3> :s/^\(\t*\)\(\/\*\)*\(.\{-\}\)\(\*\/\)*$/\1\/\*\3\*\// "коментирование строки 
nmap <F5> :e %:p:s,.h$,.X123X,:s,.cpp$,.h,:s,.X123X$,.cpp, "Переключаемся между основным файлом и заголовком
nmap <Up> gk
nmap <Down> gj
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set background=dark
set backspace=2
set cscopetag
set cscopeverbose
set display=lastline
set fileencodings=ucs-bom,utf-8,default,latin1
set helplang=ru
set ignorecase
set incsearch
set laststatus=2
set mouse=a
set printoptions=paper:a4
set ruler
set runtimepath=~/.vim,/var/lib/vim/addons,/usr/share/vim/vimfiles,/usr/share/vim/vim74,/usr/share/vim/vimfiles/after,/var/lib/vim/addons/after,~/.vim/after
set shiftwidth=4
set showbreak=...
set smartcase
set smartindent
set statusline=%<%f\ [%Y%R%W]%1*%{(&modified)?'\ [+]\ ':''}%*%=%c%V,%l\ %P\ [%n]
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set tabstop=4
set wildmenu
" vim: set ft=vim :
