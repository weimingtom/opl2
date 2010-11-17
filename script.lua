local image, menu, play, wait = pl2.setImage, pl2.showMenu, pl2.play, pl2.wait

local function text(s)
    pl2.showText(s:gsub('&([0-9])',
        function(n)
        return ({ani,imo,ani,imo})[tonumber(n)+1]:getName()
        end))
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

fore:fade(0, 0)
back:fade(0, 0)

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

camera:setUp(0, 1, 0)
camera:setFov(35)

imo:clear()
ani:clear()
room:clear()

imo2:setName('？？？', 1.0, 1.0, 1.0)
imo:setName('早苗', 1.0, 0.6, 0.6)
ani:setName('おにいちゃん', 0.6, 0.6, 1.0)

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

local script, title, h_mode, Hselect
local storyA, storyB, storyC, storyD, storyE, storyF

local skip = false

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
    pl2.quit()
end

function title()
    print'title'
    local r = rooms[randomItem(rooms)]

    room:setModels(r)
    room:setAnim(r.anim)
    room:setVisible(true)

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

    local i = menu{ 'ストーリーモード', 'とことんＨモード', '終了' }
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

    text'&3が待ち合わせた場所にやってきた。'
    text'恥ずかしいのかもじもじと身体を動かしている。'

    ani'「よしよし‥‥‥‥ちゃんと言われた通りの格好をして来たな」'
    imo'「‥‥‥‥‥‥‥‥‥‥‥‥‥‥」'

    text'こくんっと小さく頷く。'

    ani'「じゃあ早速始めようか‥‥‥‥‥」'


    pl2.setWindow(false)

    if 1 == menu{ '色々なポーズを取らせる', '‥‥えっちする' } then
        camera:setPath'cam2_3'
        ani'うん、可愛い格好だよ‥‥&3‥‥‥‥'

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

    text'俺は&3を隣に誘い静かに腰をかけた。'

    back:fade(0, 2)
    wait(2)
    return Hselect()
end

function Hselect()
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

    text'そう、この可愛い声の主は俺の妹「&3」だ。'

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
    text'‥‥‥‥で、そんな&3だが、おにいちゃんの俺が言うのもなんだがそこら辺にいる女の子よりも可愛いと思う。'
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

return script()
