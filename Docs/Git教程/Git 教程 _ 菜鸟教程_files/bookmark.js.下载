(function ($) {
  $(function () {   // ← 确保 DOM 完全就绪后再执行

    var BOOKMARK_KEY = 'bookmarks';
    var HISTORY_KEY  = 'browsingHistory';
    var MAX_ITEMS    = 20;
    var isHtmlPage   = /\.html?($|\?|#)/.test(location.href)
                       || location.pathname === '/' || location.pathname === '';

    if (typeof Storage === 'undefined') return;

    /* ── 存储 ──────────────────────────────────────────── */
    var Store = {
      get: function (key) {
        try { var s = localStorage.getItem(key); return s ? JSON.parse(s).urls : []; }
        catch (e) { return []; }
      },
      set: function (key, urls) {
        localStorage.setItem(key, JSON.stringify({ urls: urls }));
      }
    };

    /* ── 注入样式 & 面板 HTML（幂等，只注入一次） ───────── */
    if (!$('#bm-panel').length) {
      $('<style id="bm-style">').text([
        '#bm-toast-box{position:fixed;bottom:90px;left:50%;transform:translateX(-50%);z-index:9999;display:flex;flex-direction:column;align-items:center;gap:8px;pointer-events:none}',
        '.bm-toast{padding:7px 18px;border-radius:20px;font-size:13px;font-family:system-ui,sans-serif;white-space:nowrap;animation:bmIn .2s ease;box-shadow:0 2px 10px rgba(0,0,0,.15)}',
        '.bm-toast.out{animation:bmOut .2s ease forwards}',
        '.bm-toast.success{background:#eaf5ec;color:#2d7a3a}',
        '.bm-toast.warning{background:#fef3dc;color:#64854c}',
        '.bm-toast.info{background:#f2f2f2;color:#555}',
        '.bm-toast.error{background:#fdecea;color:#c0392b}',
        '@keyframes bmIn{from{opacity:0;transform:translateY(8px)}to{opacity:1;transform:translateY(0)}}',
        '@keyframes bmOut{from{opacity:1}to{opacity:0;transform:translateY(8px)}}',

        '#bm-panel-overlay{position:fixed;inset:0;background:rgba(0,0,0,.32);z-index:8000;opacity:0;pointer-events:none;transition:opacity .25s}',
        '#bm-panel-overlay.show{opacity:1;pointer-events:all}',
        '#bm-panel{position:fixed;top:0;right:0;bottom:0;width:340px;max-width:92vw;background:#faf8f4;border-left:1px solid #e8e4da;z-index:8001;display:flex;flex-direction:column;transform:translateX(100%);transition:transform .28s cubic-bezier(.4,0,.2,1)}',
        '#bm-panel.open{transform:translateX(0)}',
        '#bm-panel .bm-ph{padding:14px 16px 12px;border-bottom:1px solid #e8e4da;display:flex;align-items:center;justify-content:space-between;flex-shrink:0}',
        '#bm-panel .bm-ph h2{font:600 14px/1 system-ui,sans-serif;color:#1a1814;letter-spacing:.04em;margin:0}',
        '#bm-panel .bm-close{width:28px;height:28px;border:none;background:transparent;cursor:pointer;border-radius:6px;color:#888;font-size:16px;line-height:28px;text-align:center;padding:0;transition:background .15s,color .15s}',
        '#bm-panel .bm-close:hover{background:#eee;color:#333}',
        '#bm-panel .bm-tabs{display:flex;border-bottom:1px solid #e8e4da;padding:0 16px;flex-shrink:0}',
        '#bm-panel .bm-tab{padding:9px 0;margin-right:20px;font:13px system-ui,sans-serif;cursor:pointer;color:#999;border-bottom:2px solid transparent;transition:color .15s,border-color .15s;user-select:none}',
        '#bm-panel .bm-tab.on{color:#1a1814;border-color:#64854c}',
        '#bm-panel .bm-pane{display:none;flex-direction:column;flex:1;overflow:hidden}',
        '#bm-panel .bm-pane.on{display:flex}',
        '#bm-panel .bm-toolbar{padding:7px 16px;display:flex;justify-content:flex-end;flex-shrink:0}',
        '#bm-panel .bm-clr{font:12px system-ui,sans-serif;color:#bbb;background:transparent;border:none;cursor:pointer;padding:3px 8px;border-radius:5px;transition:background .15s,color .15s}',
        '#bm-panel .bm-clr:hover{background:#fdecea;color:#c0392b}',
        '#bm-panel .bm-list{flex:1;overflow-y:auto;padding:0 16px 16px}',
        '#bm-panel .bm-list::-webkit-scrollbar{width:3px}',
        '#bm-panel .bm-list::-webkit-scrollbar-thumb{background:#ddd;border-radius:2px}',
        '#bm-panel .bm-item{display:flex;align-items:flex-start;gap:8px;padding:10px 0;border-bottom:1px solid #f0ede6;animation:bmIn .18s ease}',
        '#bm-panel .bm-item:last-child{border-bottom:none}',
        '#bm-panel .bm-dot{width:6px;height:6px;border-radius:50%;flex-shrink:0;margin-top:5px}',
        '.bm-pane[data-pane=bookmarks] .bm-dot{background:#64854c}',
        '.bm-pane[data-pane=history] .bm-dot{background:#bbb}',
        '#bm-panel .bm-body{flex:1;min-width:0}',
        '#bm-panel .bm-title{font-size:13px;line-height:1.5;color:#1a1814;text-decoration:none;display:block;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;transition:color .15s}',
        '#bm-panel .bm-title:hover{color:#64854c}',
        '#bm-panel .bm-date{font-size:11px;color:#bbb;margin-top:3px;font-family:system-ui,sans-serif}',
        '#bm-panel .bm-del{width:24px;height:24px;border:none;background:transparent;cursor:pointer;color:#ccc;font-size:15px;line-height:24px;text-align:center;border-radius:5px;padding:0;flex-shrink:0;transition:background .15s,color .15s}',
        '#bm-panel .bm-del:hover{background:#fdecea;color:#c0392b}',
        '#bm-panel .bm-empty{padding:32px 0;text-align:center;font:13px/1.6 system-ui,sans-serif;color:#bbb}',
        '#bm-panel .bm-empty i{font-size:28px;display:block;margin-bottom:8px;color:#ddd}',

        '.fixed-btn .writer{position:relative}',
        '.fixed-btn .writer.bm-on i{color:#64854c}',
        '.bm-badge{position:absolute;top:-4px;right:-4px;width:16px;height:16px;border-radius:50%;background:#64854c;color:#fff;font:700 10px/16px system-ui,sans-serif;text-align:center;display:none}',
        '.bm-badge.show{display:block}'
      ].join('')).appendTo('head');

      $('body').append(
        '<div id="bm-toast-box"></div>' +
        '<div id="bm-panel-overlay"></div>' +
        '<div id="bm-panel">' +
          '<div class="bm-ph">' +
            '<h2><i class="fa fa-bookmark-o" style="margin-right:7px;color:#64854c"></i>我的收藏</h2>' +
            '<button class="bm-close" id="bm-close"><i class="fa fa-times"></i></button>' +
          '</div>' +
          '<div class="bm-tabs">' +
            '<div class="bm-tab on" data-tab="bookmarks"><i class="fa fa-star-o" style="margin-right:5px"></i>标记文章</div>' +
            '<div class="bm-tab" data-tab="history"><i class="fa fa-history" style="margin-right:5px"></i>浏览历史</div>' +
          '</div>' +
          '<div class="bm-pane on" data-pane="bookmarks">' +
            '<div class="bm-toolbar"><button class="bm-clr" id="bm-clr-bookmarks"><i class="fa fa-trash-o" style="margin-right:4px"></i>清空全部</button></div>' +
            '<div class="bm-list" id="bm-bookmark-list"></div>' +
          '</div>' +
          '<div class="bm-pane" data-pane="history">' +
            '<div class="bm-toolbar"><button class="bm-clr" id="bm-clr-history"><i class="fa fa-trash-o" style="margin-right:4px"></i>清空历史</button></div>' +
            '<div class="bm-list" id="bm-history-list"></div>' +
          '</div>' +
        '</div>'
      );
    }

    /* ── 给 .writer 注入徽标节点（若尚无） ─────────────── */
    if (!$('.fixed-btn .writer .bm-badge').length) {
      $('.fixed-btn .writer').append('<span class="bm-badge"></span>');
    }

    /* ── Toast ──────────────────────────────────────────── */
    function toast(msg, type, ms) {
      type = type || 'info'; ms = ms || 2600;
      var el = $('<div class="bm-toast ' + type + '">' + msg + '</div>').appendTo('#bm-toast-box');
      setTimeout(function () {
        el.addClass('out').one('animationend webkitAnimationEnd', function () { el.remove(); });
      }, ms);
    }

    /* ── 日期格式化 ─────────────────────────────────────── */
    function fmtDate(iso) {
      if (!iso) return '';
      var d = new Date(iso), p = function (n) { return ('0' + n).slice(-2); };
      return d.getFullYear() + '-' + p(d.getMonth() + 1) + '-' + p(d.getDate())
           + ' ' + p(d.getHours()) + ':' + p(d.getMinutes());
    }

    /* ── 取标题 ─────────────────────────────────────────── */
    function getTitle() {
      var h1 = document.querySelector('#content h1, article h1');
      if (h1) {
        var clone = h1.cloneNode(true);
        clone.querySelectorAll('.py-version-badge').forEach(function (el) {
          el.remove();
        });
        var text = clone.textContent.trim();
        if (text) return text;
      }
      return document.title || '无标题';
    }

    /* ── 渲染列表 ───────────────────────────────────────── */
    function renderList(listId, items, onDel) {
      var $el = $('#' + listId).empty();
      if (!items.length) {
        $el.html('<div class="bm-empty"><i class="fa fa-inbox"></i>暂无记录</div>');
        return;
      }
      var reversed = items.slice().reverse();
      for (var i = 0; i < reversed.length; i++) {
        (function (item) {
          var $row = $(
            '<div class="bm-item">' +
              '<div class="bm-dot"></div>' +
              '<div class="bm-body">' +
                '<a class="bm-title" href="' + item.url + '" target="_blank" title="' + item.title + '">' + (item.title || '无标题') + '</a>' +
                '<div class="bm-date"><i class="fa fa-clock-o" style="margin-right:3px"></i>' + fmtDate(item.date) + '</div>' +
              '</div>' +
              '<button class="bm-del" title="删除"><i class="fa fa-times"></i></button>' +
            '</div>'
          );
          $row.find('.bm-del').on('click', function () { onDel(item.url); });
          $el.append($row);
        })(reversed[i]);
      }
    }

    /* ── 刷新书签 ───────────────────────────────────────── */
    function refreshBookmarks() {
      var items = Store.get(BOOKMARK_KEY);
      renderList('bm-bookmark-list', items, function (url) {
        Store.set(BOOKMARK_KEY, Store.get(BOOKMARK_KEY).filter(function (i) { return i.url !== url; }));
        toast('已删除', 'info');
        refreshBookmarks();
      });
      var count = items.length;
      var $badge = $('.fixed-btn .writer .bm-badge');
      $badge.text(count).toggleClass('show', count > 0);
      var bookmarked = items.some(function (i) { return i.url === location.href.trim(); });
      $('.fixed-btn .writer')
        .toggleClass('bm-on', bookmarked)
        .find('i')
        .toggleClass('fa-bookmark', bookmarked)
        .toggleClass('fa-bookmark-o', !bookmarked);
    }

    /* ── 刷新历史 ───────────────────────────────────────── */
    function refreshHistory() {
      renderList('bm-history-list', Store.get(HISTORY_KEY), function (url) {
        Store.set(HISTORY_KEY, Store.get(HISTORY_KEY).filter(function (i) { return i.url !== url; }));
        toast('已删除', 'info');
        refreshHistory();
      });
    }

    /* ── 添加书签 ───────────────────────────────────────── */
    function addBookmark() {
      var title = getTitle(), url = location.href.trim();
      var urls = Store.get(BOOKMARK_KEY);
      if (urls.some(function (i) { return i.url === url; })) {
        toast('该文章已标记过了', 'warning'); return;
      }
      urls.push({ title: title, url: url, date: new Date().toISOString() });
      if (urls.length > MAX_ITEMS) urls = urls.slice(-MAX_ITEMS);
      Store.set(BOOKMARK_KEY, urls);
      toast('标记成功！', 'success');
      refreshBookmarks();
    }

    /* ── 记录历史 ───────────────────────────────────────── */
    function recordHistory() {
      var title = getTitle(), url = location.href.trim();
      var urls = Store.get(HISTORY_KEY).filter(function (i) { return i.url !== url; });
      urls.push({ title: title, url: url, date: new Date().toISOString() });
      if (urls.length > MAX_ITEMS) urls = urls.slice(-MAX_ITEMS);
      Store.set(HISTORY_KEY, urls);
    }

    /* ── 面板开关 ───────────────────────────────────────── */
    function openPanel(tab) {
      if (tab) {
        $('#bm-panel .bm-tab').removeClass('on').filter('[data-tab="' + tab + '"]').addClass('on');
        $('#bm-panel .bm-pane').removeClass('on').filter('[data-pane="' + tab + '"]').addClass('on');
      }
      refreshBookmarks();
      refreshHistory();
      $('#bm-panel').addClass('open');
      $('#bm-panel-overlay').addClass('show');
      $('body').css('overflow', 'hidden');
    }

    function closePanel() {
      $('#bm-panel').removeClass('open');
      $('#bm-panel-overlay').removeClass('show');
      $('body').css('overflow', '');
    }

    /* ── 事件绑定（全部委托到 document）────────────────── */
    $(document)
      .on('click', '#bm-close',           closePanel)
      .on('click', '#bm-panel-overlay',   closePanel)
      .on('keydown', function (e) { if (e.key === 'Escape') closePanel(); })
      .on('click', '#bm-panel .bm-tab', function () {
        var name = $(this).data('tab');
        $('#bm-panel .bm-tab').removeClass('on');  $(this).addClass('on');
        $('#bm-panel .bm-pane').removeClass('on').filter('[data-pane="' + name + '"]').addClass('on');
      })
      .on('click', '#bm-clr-bookmarks', function () {
        if (!Store.get(BOOKMARK_KEY).length) { toast('暂无标记记录', 'info'); return; }
        if (!confirm('确定清空全部标记？')) return;
        Store.set(BOOKMARK_KEY, []);
        toast('已清空标记', 'info');
        refreshBookmarks();
      })
      .on('click', '#bm-clr-history', function () {
        if (!Store.get(HISTORY_KEY).length) { toast('暂无浏览记录', 'info'); return; }
        if (!confirm('确定清空全部浏览历史？')) return;
        Store.set(HISTORY_KEY, []);
        toast('已清空历史', 'info');
        refreshHistory();
      })
      .on('click', '.fixed-btn .writer',    function () { if (isHtmlPage) addBookmark(); })
      .on('click', '.fixed-btn .bm-opener', function () { openPanel('history'); });

    /* ── 初始化 ─────────────────────────────────────────── */
    if (isHtmlPage) {
      $('.fixed-btn .writer').show();
      recordHistory();
    }
    refreshBookmarks();

  }); // end document.ready
})(jQuery);
