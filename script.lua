local image, text, menu = pl2.setImage, pl2.showText, pl2.showMenu
local play, wait = pl2.play, pl2.wait

local save, temp = pl2.save, {}

local script, title, h_mode, Hselect
local storyA, storyB, storyC, storyD, storyE, storyF

local function _(s)
    return s:gsub('&([0-3])',
        function(n)
            local t = {ani,imo,ani,imo}
            local p = tonumber(n)+1
            print(p, t[p], t[p]:getName())
            return t[p]:getName()
        end)
end
local function bgsound(...)
    return play('bgsound',...)
end
local function music(...)
    return play('music',...)
end
local function sound(...)
    return play('sound',...)
end
local function voice(...)
    return play('voice',...)
end

--------------------------------------------------------------------------------

local function print(...)
    local t = {...}
    for i, v in ipairs(t) do
        t[i] = tostring(v)
    end
    io.write(table.concat(t, '\t'), '\n')
end

local params = {
    -- camera
    c = 2, cp = 1, cl = 1, cu = 4, cv = 2,
    -- lighting
    l = -1, lv = 4, ld = 4, la = 4, ls = 4, le = 2,
    -- models/animation
    m = -1, mc = 3, ml = 2, mp = 2, ma = 3, md = 2, mh = 3, mv = 2, mV = 2,
    mm = 2, mb = 2, mn = 2, mr = 0,
    -- text/menus
    Wl = 1, W = 1, n = 1, K = 0, r = 0, i = 2, I = 0,
    -- graphics/effects
    T = 1, g = 1, w = 1, f = 3,
    -- sound/music
    M = 2, MA = 2, MS = 2, o = 1, oR = 5,
    -- flow control
    G = 1, J = 1, Q = 0, q = 1, S = 3, E = 4,
    -- unknown commands
    X = 0, Z = 1,
    }

local done, scripts = {}, {}

local hdr = string.rep('=',80)
local sep = string.rep('-',80)

function string:gfind(pattern, init, plain)
    if not init then init = 1 end
    local function f()
        local t = { self:find(pattern, init, plain) }
        if not t[2] then return end
        init = t[2] + 1
        return unpack(t)
    end
    return f
end

function string:split(d, m, k)
    if not m then m = #self end
    --print('split: m == ' .. m)
    local r, i, n = {}, 1, 0
    for s, e, p in self:gfind(d) do
        r[#r+1] = self:sub(i, s-1)
        if k then r[#r+1] = self:sub(s, e) end
        i = e + 1 ; n = n + 1
        if n > m then break end
    end
    if i <= #self then
        r[#r+1] = self:sub(i)
    end
    return r
end

local function parseScript(name)
    if scripts[name] then
        return
    end

    scripts[name] = { labels = {}, commands = {} }

    local lbls = scripts[name].labels
    local cmds = scripts[name].commands

    --print(hdr)
    --print(name)
    --print(hdr)

    local scr = pl2.loadScript(name)

    if not scr then
        --print'Load error'
        --print()
        return
    end

    local x, y = 1, #scr

    local line = 1

    local c, l
    while x <= y do
        c = scr:sub(x,x)
        x = x + 1

        if c == '%' then
            local xx = x
            local cmd, arg = scr:sub(x, xx), {}

            if params[cmd] then
                while params[cmd] and xx <= y do
                    xx = xx + 1
                    cmd = scr:sub(x, xx)
                end
            else
                x = xx + 1
            end

            cmd = scr:sub(x, xx - 1)
            x = xx

            if params[cmd] and params[cmd] >= 0 then
                local t = '\t%' .. cmd

                if params[cmd] > 0 then
                    while scr:sub(xx, xx) ~= ';' do
                        xx = xx + 1
                    end
                    arg = scr:sub(x, xx - 1):split(',', params[cmd])

                    t = t .. table.concat(arg,',') .. ';'
                end

                print(t)

                if cmd == 'G' then
                    parseScript(arg[1])
                    --print(sep)
                    --print('>>> Back to ' .. name)
                    --print(sep)
                end
            else
                print(('%s(%d): warning: unknown command "%s"'):format(name, line, cmd))
            end
        elseif c == ':' then
            local xx = x
            while scr:sub(xx, xx) ~= ';' do
                xx = xx + 1
            end
            x = xx + 1
            lbls[scr:sub(x, xx-1)] = x
        elseif c == '#' then
            repeat
                x = x + 1
                c = scr:sub(x, x)
            until c == '\013' or c == '\010'
        elseif c == '\013' or c == '\010' then
            if not(c == '\010' and l == '\013') then
                line = line + 1
            end
        elseif c ~= '\009' then
        end

        l = c
    end
end

if false then
    local t = io.output()
    io.output('parseScript.log')
    parseScript('script')
    io.output(t)
    return
end

--------------------------------------------------------------------------------

imo2:setName('？？？', 1.0, 1.0, 1.0)
imo:setName('早苗', 1.0, 0.6, 0.6)
ani:setName('おにいちゃん', 0.6, 0.6, 1.0)
ani:setBlack(true)

--------------------------------------------------------------------------------

local IMO = {
    BODY    =  1,
    EYE     =  2,
    UNDER_A =  3,
    UNDER_B =  4,
    SOCKS   =  5,
    COS_A   =  6,
    COS_B   =  7,
    HEAD    =  8,
    FACE    =  9,
    NECK    = 10,
    ARM     = 11,
    SHOES   = 12,
    HAIR    = 13,
}

--------------------------------------------------------------------------------

local function randomItem(tbl)
    return tbl[math.random(1, #tbl)]
end

--------------------------------------------------------------------------------

local rooms = {
    'bedroom',
    'gym_storage',
    'train',
    --'classroom',
    --'beach',
    --'furo',

    bedroom = {
        [1]='room_01',
    },
    gym_storage = {
        [1]='room_02',
    },
    train = {
        [1]='room_03',
        [2]='room_03A',
        anim='room_03A',
    },
    furo = {
        [1]='room_huro01',
    },
    classroom = {
        [1]='class01',
    },
    beach = {
        [1]='beach01a',
        [2]='beach01b',
        anim='beach01b',
    },
}

--------------------------------------------------------------------------------

local skip = save.skip

function script()
    print'script'
    pl2.setQuit(false)

    fore:fade(0, 0)
    back:fade(0, 0)

    music(nil, 0)
    bgsound(nil, 0)
    voice(nil, 0)

    camera:setPath(nil, false)
    camera:setPoint(nil)

    pl2.setWindow(false)
    if not skip then
        skip = true
        image'op1'
        fore:fade(1, 2)
        wait(6)
        fore:fade(0, 2)
        wait(2)
        image'op2'
        fore:fade(1, 2)
        voice'0002'
        wait(2)
    end

    return title()
end

function quit()
    pl2.setWindow(false)
    pl2.setQuit(false)
    voice'0011'
    music(nil, 2)
    fore:fade(0, 2)
    wait(2.25)
    --pl2.quit()
end

function title()
    print'title'
    local r = (save.title_room or 0) % 3 + 1
    save.title_room = r

    camera:setUp(0, 1, 0)
    camera:setFov(35)

    imo:clear()
    ani:clear()
    room:clear()

    camera:setLocked(false)

    light1:setPosition(-1, -1, -1)
    light1:setDiffuse(0.6, 0.6, 0.6)
    light1:setAmbient(0.8, 0.8, 0.8)
    light1:setSpecular(1.0, 1.0, 1.0)
    light1:setEnabled(true)

    light2:setPosition(1, 1, 1)
    light2:setDiffuse(0.2, 0.2, 0.2)
    light2:setAmbient(0.2, 0.2, 0.2)
    light2:setSpecular(0.8, 0.8, 0.8)
    light2:setEnabled(true)

    imo:setVisible(false)
    ani:setVisible(false)

    if save.title_room == 3 then
        room:setModels{ 'room_03', 'room_03A' }
        room:setAnim'room_03A'
        room:setVisible(true)
    elseif save.title_room == 2 then
        room:setModels{ 'room_02' }
        room:setVisible(true)
    else
        room:setModels{ 'room_01' }
        room:setVisible(true)
    end

    camera:setPath('A1cam1', true)

    fore:fade(0, 1)
    wait(1)
    image(nil)

    music('BGM01',0)
    voice'0004'

    fore:fade(1, 2)
    back:fade(1, 2)
    wait(1)

    pl2.setQuit(true)

    local i = menu{
        'ストーリーモード',
        save.unlocked_h_mode
            and 'とことんＨモード'
            or '*とことんＨモード',
        '終了' }
    print('selected',i)

    return ({ storyA, h_mode, quit })[i]()
end

function h_mode()
    print'h_mode'

    voice'0008'
    fore:fade(0, 2)
    music(nil, 4)
    wait(2)

    pl2.setTitle(false)
    pl2.setQuit(true)

    imo:setModels{
        'imo_bodyA_00',
        'imo_eye_01',
        'imo_underA_01A',
        'imo_underA_01B',
        [13]='imo_hairB_06',
    }
    imo:setAnim'event_01'

    ani:setModels{ 'ani_bodyB_00' }
    ani:setVisible(false)

    temp.h_mode = true

    camera:setPath(nil)
    music('BGM05',0)

    --pl2.showHModeSetup(true)
    room:setModels{ 'room_01' }
    imo:setVisible(true)
    pl2.setWindow(true)
    text'とことんＨモード'
    pl2.setWindow(false)

    music(nil, 2)
    fore:fade(0, 2)
    wait(2)
    --pl2.showHModeSetup(false)

    imo:setAnim'event_02'
    imo:setPoint'loc_pos00'
    camera:setPoint'loc_pos00'
    camera:setPath'B1cam2'

    pl2.setWindow(true)
    back:fade(1, 0)
    fore:fade(1, 2)

    text(_'&3が待ち合わせた場所にやってきた。')
    text'恥ずかしいのかもじもじと身体を動かしている。'

    ani'「よしよし‥‥‥‥ちゃんと言われた通りの格好をして来たな」'
    imo'「‥‥‥‥‥‥‥‥‥‥‥‥‥‥」'

    text'こくんっと小さく頷く。'

    ani'「じゃあ早速始めようか‥‥‥‥‥」'


    pl2.setWindow(false)

    if 1 == menu{ '色々なポーズを取らせる', '‥‥えっちする' } then
        camera:setPath'cam2_3'
        ani(_'うん、可愛い格好だよ‥‥&3‥‥‥‥')

        ani'それじゃー、もっと良く見せて？'

        local txt = { '通常立ち', 'もじもじ', 'ぷんぷん', 'えへへ〜',
                      'しょんぼり', 'うっそ〜！', 'ウマウマー', '‥‥えっちする' }
        local evt = { 'event_01', 'event_02', 'event_03', 'event_04',
                      'event_05', 'event_06', 'umauma00_F' }

        repeat
            pl2.setWindow(false)

            local i = menu(txt)

            if i < 8 then
                imo:setAnim(evt[i])
                wait(3)
                pl2.setWindow(true)
                text(txt[i])
            end
        until i == 8
    end

    text(_'俺は&3を隣に誘い静かに腰をかけた。')

    back:fade(0, 2)
    wait(2)
    return Hselect()
end

function Hselect()
    pl2.setTitle(false)
    pl2.setQuit(true)

    if temp.h_mode then
        if temp.zen_step ~= 4 then
            temp.zen_step = 4

            imo:setPoint'loc_pos01'
            ani:setPoint'loc_pos01'
            camera:setPoint'loc_pos01'
            ani:setVisible(true)

            music('BGM02', 0)

            imo:setAnim'zen_a01_F'
            ani:setAnim'zen_a01_M'

            camera:setPath'zen_a01_1'

            back:fade(1, 1)
        end
    else
--[==[
#＜初めて来たか？の分岐＞

    %E18,=,1,h_mode;
    %E11,=,0,zen01a;
--]==]

        if temp.f_arousal >= 100 or temp.m_arousal >= 100 then
            return sex_sel()
        end

        if temp.m_arousal >= 50 then
            if temp.ani_state ~= 3 then
                temp.ani_state = 3

                back:fade(0, 1)
                wait(1)
                ani:setModels{ 'ani_bodyA_00' }
                pl2.setWindow(true)

                text'これ以上我慢が出来ない。'
                text'下半身を覆う布の下で俺の半身が爆発しそうだ。'
                text'それを解放する為にズボンを脱ぎ、俺は全裸になった‥‥‥‥'

                --%S65,=,1;
                --%S14,=,1;
            end
        elseif temp.m_arousal >= 17 then
            if temp.ani_state ~= 2 then
                temp.ani_state = 2

                back:fade(0, 1)
                wait(1)
                ani:setModels{ 'ani_bodyC_00' }
                pl2.setWindow(true)

                text'興奮で身体が燃えるように熱い。'
                text'汗が服に張り付く感触が不快だ。'
                text'だから、俺はシャツを脱ぐ事にした‥‥‥‥'

                --%S65,=,1;
                --%S13,=,1;
            end
        else
            if temp.ani_state ~= 1 then
                temp.ani_state = 1

                ani:setModels{ 'ani_bodyB_00' }
            end
        end
    end


--[==[
#＜前戯か本番か＞
:zensex;
    %E16,=,0,zen00;
    %E16,=,1,sex00;





#＜前戯＞

:zen00;
    %W0;



%E129,>=,17,zen00b;



:zen00a;
    %W0;

    %i話す,zen01;
    %i抱きしめる,zen02;
    %iキスをする,zen03;

    %E65,=,1,nugi1;

    %i‥‥服を脱ぐ＆脱がす,nude00;


    %E18,=,0,b1;
    %i次のステップへ‥‥‥,h1;

:nugi1;
    %S65,=,0;

:b1;


    %I


:zen00b;
    %W0;

%E129,>=,50,zen00c;

    %i胸を揉む,zen04;
    %i胸を舐める,zen05;
    %iあそこを触る,zen06;
    %iあそこを舐める,zen07;
    %i足でしてもらう,ashi01;
    %i足でされる,ashi02;


    %E65,=,1,a2;
    %i‥‥服を脱ぐ＆脱がす,nude00;


    %E18,=,0,b2;
    %i次のステップへ‥‥‥,h2;

:a2;

    %S65,=,0;


:b2;





    %I



:zen00c;
    %W0;

    %i舐めてもらう,zen08;
    %i口に含んでもらう,zen09;
    %iオナニーをさせる,zen10;
    %i両足くにくに,ashi03;
    %i手でしてもらう,tekoki01;
    %iパイズリ,paizuri01;
    %iヴァイブプレイ,vib01;
    %E65,=,1,a3;
    %i‥‥妹と一つになる,sex00;
    %i‥‥服を脱ぐ＆脱がす,nude00;



:a3;
    %S65,=,0;

    %I



:paizuri01;
    %f0,0,60;
    %w60;
    %Gpaizuri_01;



:zen01;
    %W0;
    %f0,0,60;
    %w60;
    %S11,=,1;
    %Gzen_01;


:zen01a;
    %W0;
    %f0,0,60;
    %w60;

    %S11,=,1;
    %S12,=,1;
    %Gzen_01;

:zen02;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_02;


:zen03;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_03;


:zen04;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_04;


:zen05;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_05;


:zen06;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_06;


:zen07;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_07;


:zen08;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_08;


:zen09;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_09;


:zen10;
    %W0;
    %f0,0,60;
    %w60;
    %Gzen_10;







#＜おかわり、追加分＞


:ashi01;
    %W0;
    %f0,0,60;
    %w60;
    %Gok_ashi01;


:ashi02;
    %W0;
    %f0,0,60;
    %w60;
    %Gok_ashi02;



:ashi03;
    %W0;
    %f0,0,60;
    %w60;
    %Gok_ashi03;


:tekoki01;
    %W0;
    %f0,0,60;
    %w60;
    %Gok_tekoki01;



:vib01;
    %W0;
    %f0,0,60;
    %w60;
    %Gok_vib01;






#＜前戯、服脱ぎ＞



:nude00;

    %n;
    text(_'&3の綺麗な肌が見たい‥‥‥')

    そう思い、彼女の服を脱がそうと手をのばした‥‥‥%K

    %E61,=,1,up;
    %i上半身の服を脱がす,nude01;

:up;
    %E62,=,1,dw;
    %i下半身の服を脱がす,nude02;

:dw;
    %E61,=,0,br1;
    %E63,>=,1,br1;
    %iブラを半脱ぎ,nude03;

:br1;

    %E64,>=,1,pn1;
    %iパンティを半脱ぎ,nude04;

:pn1;
    %E63,=,0,br2;
    %E63,=,2,br2;
    %iブラを脱がす,nude05;

:br2;
    %E64,=,0,pn2;
    %E64,=,2,pn2;
    %iパンティを脱がす,nude06;

:pn2;
    %E68,=,1,pn3;

    %i靴を脱がす,nude10;

:pn3;

    %i‥‥脱がすのを止める,nude07;

    %E18,=,0,an1;
    %E14,=,1,an1;

    %i‥‥自分の服を脱ぐ,ani01;

:an1;

    %I





:nude01;
    %f0,0,30;
    %w30;
    %md0,5;

    %S61,=,1;


    %n&0;「暑いから脱ごうな‥‥‥」%K

    %n;
    text(_'そう言って俺は&3の上着を丁寧に脱がしてあげた‥‥‥')

    %f0,255,30;
    %w30;
    %Jzen00;

:nude02;
    %f0,0,30;
    %w30;
    %md0,6;

    %S62,=,1;

    %n;
    text(_'&3の下半身に手を伸ばし服を掴む。')

    text(_'&3は赤くなりながらも、俺に抵抗せず服を脱がされていく‥‥‥')



    %f0,255,30;
    %w30;
    %Jzen00;

:nude03;
    %f0,0,30;
    %w30;

    %S63,=,1;

    %mh0,0,1;   #＜ブラを半脱ぎ＞

    %n;
    ブラを少しだけずらした。%K

    すると、柔らかい桃の果実が姿を現した。%K

    %f0,255,30;
    %w30;
    %Jzen00;


:nude04;
    %f0,0,30;
    %w30;

    %S64,=,1;
    %S15,=,1;

    %mh0,1,1;   #＜パンツを半脱ぎ＞

    %mc0,0,1;
    %mc0,1,0;
#   %mc0,2,0;

    %n;
    text(_'&3の大事な部分を覆う布をずらした。')

    現れた秘裂は汗かそれとも別の分泌物でてらてら光ってる。%K

    %f0,255,30;
    %w30;
    %Jzen00;

:nude05;
    %f0,0,30;
    %w30;

    %mh0,0,2;
    %S63,=,2;
    %S15,=,1;


    %mc0,0,1;
    %mc0,1,0;
    %mc0,2,0;



    %n;
    この綺麗な果実をしっかり堪能したいと思った。%K

    だから俺は下着を剥ぐように脱がせた。%K

    %f0,255,30;
    %w30;
    %Jzen00;

:nude06;
    %f0,0,30;
    %w30;

    %S15,=,1;
    %S64,=,2;

    %mh0,1,2;

    %mc0,0,1;
    %mc0,1,0;
    %mc0,2,0;

    %n;
    text(_'&3のおしりは綺麗だと思った。')

    だから下半身を覆う下着を脱がせる‥‥‥%K

    %f0,255,30;
    %w30;
    %Jzen00;


:nude07;
    %f0,0,30;
    %w30;

    %n;
    やっぱり脱がすのは止めよう。%K

    %f0,255,30;
    %w30;
    %Jzen00;



:ani01;
    %f0,0,30;
    %w30;


    %E13,=,1,an02;
    %E12,=,1,an01;


:an01;
    %S12,=,0;
    %S13,=,1;

    %n;
    我慢出来ない‥‥‥‥俺はシャツを脱いだ。%K

    %S65,=,1;
    %ml1,ani_bodyC_00;
#   %f0,255,30;
    %w30;



    %Jzen00;


:an02;

    %S13,=,0;
    %S14,=,1;


    %n;
    俺はズボンとパンツを脱ぎ、自分の下半身の解放した。%K

    %S65,=,1;
    %ml1,ani_bodyA_00;
#   %f0,255,30;
    %w30;


    %Jzen00;


:nude10;
    %f0,0,30;
    %w30;

    %S68,=,1;

    %md0,11;

    %n;
    text(_'&3の小さな足が好きだ')

    だから靴を脱がせた‥‥‥%K

    %f0,255,30;
    %w30;
    %Jzen00;









#＜本番＞


:sex00;
    %W0;
    %Gsex_sel;




#＜初めてのエッチ＞
:sex00a;

    %E18,=,1,h3;


#   <ストーリーモード導入>


    %f0,0,120;
    %w120;
    %W1;

    我慢の限界だった。%K

    text(_'&3と一つになりたい衝動をこれ以上押さえ切れない。')

    ani(_'「‥‥‥‥‥‥‥‥&3‥‥‥」')

voice'0283'
 %n&1;「うん‥‥‥‥‥いいよ‥‥‥おにいちゃん‥‥‥‥わたしは‥‥‥‥‥大丈夫だから‥‥‥‥‥」%K

    %n&0;「‥‥‥‥‥‥わかった」%K
    %n;
    亀頭を割れ目にあてがう。%K

    お互いの粘液が絡み合い、くちゅりっと音を立てる。%K

    体重をかけ、ずぶずぶと太い肉塊を膣内に収めていく。%K


    %mp0,loc_pos02;
    %mp1,loc_pos02;
    %cploc_pos02;
    %mv1,1;


    %mm0,sex_a01_F;
    %mm1,sex_a01_M;
    %csex_a01_1,0;
    %f0,255,60;
    %w60;





voice'0284'
 %n&1;「くうっ！！‥‥‥‥‥‥‥‥‥あっ‥‥‥‥‥‥‥あっ‥‥‥」%K
    %n;
    少し挿入したところで軽い抵抗を感じる。%K

    text(_'&3の処女膜だ。')

voice'0285'
 %n&1;「あああああっ！！‥‥‥‥‥‥い‥‥痛い‥‥‥‥‥痛いよぅ‥‥‥‥」%K
    %n;
    可愛そうにも思えたが、一つになりたいと言う欲望の方が勝っていた。%K

    俺は構わずに腰を進める。%K

voice'0286'
 %n&1;「あああっ！！‥‥‥‥おにぃちゃんっ！！‥‥‥‥‥おにぃちゃんっ！！」%K
    %n;
    text(_'異物に対する恐怖と痛みに耐える為、&3が俺を呼び続ける。')

    我慢の限界かと思われたその時‥‥‥‥‥‥‥%K

    ぷつっ‥‥‥%K

    突然抵抗が消えた。%K

    先ほどまでの圧迫感は消え、代わりに全体を包む温もりが陰茎を伝ってくる。%K

voice'0287'
 %n&1;「あああああっ！！‥‥‥‥‥‥‥‥はあっ‥‥‥‥‥‥‥はあっ‥‥‥‥‥‥‥」%K

    %n;
    text(_'‥‥‥‥‥‥&3は女になった。')

    見ると瞳から涙が溢れ出ていた。%K

    %n&0;「‥‥‥‥‥痛いのか？」%K

voice'0288'
 %n&1;「ううん‥‥‥‥‥ちがうの‥‥‥‥‥うれしいの‥‥‥‥‥」%K
    %n;
    痛くない筈はない。%K

    text(_'それなのに&3は笑顔でそう答える‥‥‥‥')

    自分の中に愛おしさが堪らなくこみ上げて来るのがわかった。%K

voice'0288A'
%n&1;「へ、平気だから‥‥‥‥‥‥‥続けて‥‥‥‥いいよ‥‥‥‥‥‥‥」%K

    %n&0;「‥‥‥‥ああ」%K
    %n;
    今は精一杯優しくしてやろう‥‥‥‥‥俺はそう思った。%K


    %S17,=,1;

    %Jsex00b;











:h1;

    %W0;
    %S129,=,30;

    %Jzensex;

:h2;
    %W0;
    %S129,=,70;

    %Jzensex;


:h3;
    %W0;
    %S16,=,1;
    %S17,=,1;

    %Jsex00;








#＜エンディング判定＞

:end00;

    %Gsex_sel;


--]==]

    return script()
end

function storyA()
    print'storyA'

    voice'0007'
    music(nil, 4)

    fore:fade(0, 2)
    wait(2)

    back:fade(0, 0)
    pl2.setTitle(false)
    pl2.setQuit(true)

    ani:setModels{ 'ani_bodyB_00' }
    ani:setPoint'loc_pos00'

    imo:setVisible(false)
    ani:setVisible(false)
    room:setVisible(false)

    room:setModels{ 'room_01' }
    camera:setPoint'loc_pos00'
    camera:setPath('A1cam1', true)

    fore:fade(1, 0)
    bgsound'se09'
    pl2.setWindow(true)
    back:fade(1, 1)
    wait(1)

    text'‥‥‥‥‥勝った！'
    text'俺は自由だ！！'
    text'英語で言うとフリーダム。'
    text'まあー、何に勝ったかと言えば受験戦争だ。'
    text'つまりは大学に受かったのである。'
    text'大学が始まって数ヶ月経った今も、俺は毎日この喜びを満喫している。'
    text'一昨年までは勉強なんて、なーんもする気の無かった\n俺だが今年は一味違っていた。'
    text'魔女っ娘が主人公の「たんもえ」と言うナイスな参考書が俺のやる気を刺激、嘘のように勉強がはかどったのだ。'
    text'いい世の中になった物である。'
    text'ありがとう「たんもえ」、ありがとうゆとり教育。'
    text'日本の将来が心配です。'
    text'‥‥‥‥‥さておき。'
    text'自由も手に入れたことだし、これで心置きなく積みゲーになっていた「Ｈａｔｅ／ｓｔａｙ　ｔｉｇｈｔ」\n（初回版）が遊べるってもんだ。'
    text'迫り来る６人の魔法使いに酷く嫌われながら‥‥\n（ここが　Ｈａｔｅ）'
    text'命がけの試練に耐え抜き‥‥\n（ここが　ｓｔａｙ　ｔｉｇｈｔ）'
    text'愛を勝ち取ると言う壮絶なエロゲーである。'
    text'総プレイ時間は６０時間とも言われる大作ゲームだが\n今の俺は腐るほど時間がある。'
    text'コーラとカップ麺は三日分用意してある。\nもはや徹夜もじさない覚悟だ。'
    text'クリアするまでこの部屋を一歩も外に出ないぞっ\n後はぶっ倒れるまでやるだけだ！（何を？）'
    text'レッツゴー！！目眩く官能の世界へ！！'
    text'誰にも俺の邪魔をさせないゼェ〜〜！！'

    sound'se25'

    text'っと、思った所で早速邪魔が入った。'

    voice'0037'
    imo2'「おにいちゃ〜ん、居るの〜？」'

    text'ドアの向こうから俺を呼ぶ声がする。'
    text'ちぃ〜こんな時に来なくてもいいだろう、\nタイミング悪すぎるぞあいつ。'
    text'ここは一つ‥‥‥‥‥\n大学に受かったこの俺の冴えた頭脳であいつをやり過ごすとするか。'

    voice'0038'
    imo2'「‥‥‥‥‥あれ？居ないの？」'

    ani'「‥‥‥‥‥‥‥‥‥‥‥」'
    ani'「‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥いないよ〜」'

    voice'0039'
    imo2'「あ、そっか‥‥‥‥じゃーまた後で来るね〜」'

    sound'se17'

    text'ドアの前から足音が遠ざかる。'
    text'‥‥ふう、危ない。\nとっさの機転で俺のプライベートタイムが守られたようだ。'
    text'では気を取り直して、レッツゴー！！\n目眩く官能の‥‥‥'

    sound'se18'

    voice'0040'
    imo2'「‥‥‥って、居るじゃない！！」'

    ani'「ふふおっ！！」'

    text'驚きのあまり不思議な言葉を発してしまった。'
    text'完璧にやり過ごせたと思ったのだが、居ることを気づかれてしまったようだ。'
    text'やるな、流石は俺の妹だ。'

    voice'0041'
    imo'「ねぇ〜おにいちゃん〜〜居るんでしょ〜〜？\n‥‥‥‥返事してよ〜〜」'

    bgsound(nil)
    back:fade(0, 1)
    camera:setLocked(true)

    text(_'そう、この可愛い声の主は俺の妹「&3」だ。')

    music(nil, 2)

    text'‥‥‥‥‥‥‥‥‥‥‥‥‥'
    text'‥‥‥‥‥‥あれ？'
    text'あいつどんな容姿してたっけ？'
    text'アルツを疑いながらも、とりあえず妹を思い出して見る。'
    text'まずは髪型は茶色の長いツィンテールだ。'
    text'うん、それは間違いない。'
    text'それから‥‥‥‥'

    local face, eyes, body

    face = ({ 'A', 'B', 'C' })[menu{
        '普通に綺麗な顔つき', '意思の強そうな顔つき', '優しくて落ち着く顔つき' }]

    text'そして瞳の色は‥'

    pl2.setWindow(false)

    while not eyes do
        eyes = ({ 0, 1, 2, 3, 4, 6 })[menu{
            '燃える赤色だ', '大自然の緑色だ', 'おいしそうなオレンジ色だ', '目立つ黄色だ',
            '深いコバルトブルーだ', '自然な茶色だ', 'いや、他の色だ‥' }] or
               ({ 7, 8, 9, 10, 11 })[menu{
            '馴染みのある黒だ', '怪しい紫色だ', 'カッコいい銀色だ', '珍しい黄金色だ',
            '綺麗な水色だ', 'いや、他の色だ‥' }]
    end

    pl2.setWindow(true)

    text'顔はそんな感じだったと思う。'
    text'ちなみに身体は‥‥'

    body = ({ 0, 1, 2, 3, 4 })[menu{
        '普通の肌色だ', '病弱な肌色だ', '健康的な小麦色だ', '魅惑のビキニ焼けだ',
        'マニア推奨のスク水焼けだ' }]

    imo:setModels{
        ('imo_body%s_%02d'):format(face, body),
        ('imo_eye_%02d'):format(eyes),
        [13] = 'imo_hairB_06' }
    imo:setPoint'loc_pos00'
    imo:setAnim'event_01'

    imo:setVisible(true)
    camera:setPath('camA', false)

    back:fade(1, 1)

    text'‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥'

    back:fade(0, 1)
    wait(1)

    imo:setVisible(false)

    music(nil, 2)

    camera:setPath('A1cam1', true)
    imo:setAnim'event_06'
    imo:setAnim'event_02'

    bgsound'se09'
    back:fade(1, 1)
    camera:setLocked(false)
    wait(1)

    text'そうそう〜こんな感じだ。（何故か裸）'
    text(_'‥‥‥‥で、そんな&3だが、おにいちゃんの俺が言うのもなんだがそこら辺にいる女の子よりも可愛いと思う。')
    text'ただ、いつも何かと理由を見つけては俺の部屋にやってくるのが困りものだ。'
    text'それはもう自分の部屋よりも俺の部屋で過ごす時間の方が多いんじゃないか？って思えるぐらい長く居座る。'
    text'普段なら適当に相手するし、どこに居ようと別に構わないのだが‥‥‥‥'
    text'俺はエロゲーを遊ばなくてはいけないと言う崇高な使命があり義務があり責任がある。'
    text'つまり、今は邪魔だ。'

    voice'0042'
    imo'「もしも〜し？おにいちゃん居るんでしょ？\n‥‥‥‥入るよ〜？」'

    ani'「あっ、おい、ちょっと待て！！」'

    back:fade(0, 0.2)

    sound'se07'

    text'制止の声も空しく妹はドアを開けて部屋に入って来た。'

    bgsound(nil)

    local outfits = {
        {   [ 3] = 'imo_underA_01A',
            [ 4] = 'imo_underA_01B',
            [ 5] = 'imo_socksB_01',
            [ 6] = 'imo_cosA_01A',
            [ 7] = 'imo_cosK_03B',
            [12] = 'imo_shoesB_03' },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_05A',
            [7] = 'imo_cosA_05B',
            [13] = 'imo_shoesB_04' },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_06A',
            [7] = 'imo_cosK_03B',
            [13] = 'imo_shoesB_04' },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [6] = 'imo_cosA_06A',
            [7] = 'imo_cosA_01B',
            [13] = 'imo_shoesB_01', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_01A',
            [7] = 'imo_cosA_04B',
            [13] = 'imo_shoesB_04' },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_00',
            [6] = 'imo_cosK_06A',
            [7] = 'imo_cosK_00B',
            [12] = 'imo_shoesD_00', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksA_05',
            [6] = 'imo_cosA_07A',
            [7] = 'imo_cosA_07B',
            [12] = 'imo_shoesB_01', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_03A',
            [7] = 'imo_cosK_05B',
            [12] = 'imo_shoesB_03', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksC_01',
            [6] = 'imo_cosK_07A',
            [7] = 'imo_cosA_03B',
            [12] = 'imo_shoesB_03', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [6] = 'imo_cosK_05A',
            [7] = 'imo_cosK_06B',
            [12] = 'imo_shoesB_01', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_04A',
            [7] = 'imo_cosA_04B',
            [12] = 'imo_shoesD_05', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_00A',
            [7] = 'imo_cosA_06B',
            [12] = 'imo_shoesB_00', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_00A',
            [7] = 'imo_cosK_07B',
            [12] = 'imo_shoesB_00', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_05A',
            [7] = 'imo_cosK_07B',
            [12] = 'imo_shoesB_04', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [6] = 'imo_cosA_06A',
            [7] = 'imo_cosK_06B',
            [12] = 'imo_shoesB_01', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_04A',
            [7] = 'imo_cosK_04B',
            [12] = 'imo_shoesB_04', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosK_07A',
            [7] = 'imo_cosK_01B',
            [12] = 'imo_shoesB_04', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_01A',
            [7] = 'imo_cosA_07B',
            [12] = 'imo_shoesB_04', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [5] = 'imo_socksB_01',
            [6] = 'imo_cosA_04A',
            [7] = 'imo_cosK_04B',
            [12] = 'imo_shoesB_04', },
        {   [3] = 'imo_underA_01A',
            [4] = 'imo_underA_01B',
            [6] = 'imo_cosA_03A',
            [7] = 'imo_cosA_03B',
            [12] = 'imo_shoesD_00', },
        }

    imo:setModels(randomItem(outfits), true)

    imo:setVisible(true)

    camera:setPath('B1cam1', false)

    music'BGM04'
    back:fade(1, 1)
    wait(1)

    imo'「‥‥‥‥‥‥‥‥‥‥‥‥‥」'

    imo:setAnim'event_06'
    camera:setPath('cam01', false)

    voice'0043'
    imo'「‥‥‥‥‥‥‥‥‥‥‥あっ‥‥‥」'

    return storyB()
end

function storyB()
    print'storyB'
--[==[
    room:setVisible(true)
    imo:setVisible(true)
    pl2.setWindow(true)

    room:setModels{ 'eventBG_01' }
    camera:setLocked(true)
    imo:setPoint'loc_pos00'
    ani:setPoint'loc_pos00'
    camera:setPoint'loc_pos00'
    camera:setPath('cam05', false)
    back:fade(1, 2)
    wait(2)

    voice'0085'

    imo'「おにいちゃん‥‥‥‥ここって‥‥‥‥」'

    ani'「そう！！ここは乙女の園、レディスインナーウェア専門店「ボディースクラッチ」だ！！」'

    text'最近はこういった下着の専門店が増えて来てる。'

    text'流石の俺も一人では入れないこのお店も、今日は妹と言うカモフラージュがあるから大丈夫だ。'

    --camera:setPath('cam03', false)
--]==]
    return storyC()
end

function storyC()
    print'storyC'

    return storyD()
end

function storyD()
    print'storyD'

    return storyE()
end

function storyE()
    print'storyE'

    return storyF()
end

function storyF()
    print'storyF'

    return script()
end

local function test()
    pl2.setQuit(true)
    back:fade(1, 0)
    fore:fade(1, 0)

    --camera:setPath('A1cam1', true)
    camera:setEye(0, 10, -20)
    camera:setFocus(0, 10, 0)
    camera:setUp(0, 1, 0)
    camera:setFov(35)

    light1:setPosition(-1, -1, -1)
    light1:setDiffuse(0.6, 0.6, 0.6)
    light1:setAmbient(0.8, 0.8, 0.8)
    light1:setSpecular(1.0, 1.0, 1.0)
    light1:setEnabled(true)

    light2:setPosition(1, 1, 1)
    light2:setDiffuse(0.2, 0.2, 0.2)
    light2:setAmbient(0.2, 0.2, 0.2)
    light2:setSpecular(0.8, 0.8, 0.8)
    light2:setEnabled(true)

    room:setModels{ 'room_03', 'room_03A' }
    room:setAnim'room_03A'
    room:setVisible(true)

    imo:setModels{
        [1]="imo_bodyA_01",
        [2]="imo_eye_vocaloid_00",
        [3]="imo_underA_06A",
        [4]="imo_underA_06B",
        [6]="imo_cos_vocaloid_00A",
        [7]="imo_cos_vocaloid_00B",
        [8]="imo_head_vocaloid_00",
        [11]="imo_arm_vocaloid_00",
        [12]="imo_shoes_vocaloid_00",
        [13]="imo_hair_vocaloid_00",
    }
    imo:setAnim'zen_a01_F'
    imo:setPoint'loc_pos01'
    imo:setVisible(true)

    text'test'

    voice'0011'
    music(nil, 2)
    fore:fade(0, 2)
    wait(2.25)
end

if test then return test() end

return script()
