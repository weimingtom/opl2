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
        [1]="room_01",
    },
    gym_storage = {
        [1]="room_02",
    },
    train = {
        [1]="room_03",
        [2]="room_03A",
        anim="room_03A",
    },
    furo = {
        [1]="room_huro01",
    },
    classroom = {
        [1]="class01",
    },
    beach = {
        [1]="beach01a",
        [2]="beach01b",
        anim="beach01b",
    },
}

--------------------------------------------------------------------------------

local init, title, h_mode, storyA, storyB, storyC, storyD, storyE, storyF

local skip = false

function script()
    pl2.setQuit(false)
    
    fore:fade(0, 0)
    back:fade(0, 0)
    
    pl2.play('music', nil, 0)
    pl2.play('bgsound', nil, 0)
    pl2.play('voice', nil, 0)
    
    camera:setPath(nil, false)
    camera:setPoint(nil)
    
    pl2.setWindow(false)
--[[    
    if not skip then
        skip = true

        pl2.setImage('op1')
        fore:fade(1, 2)
        pl2.wait(6)
        fore:fade(0, 2)
        pl2.wait(2)
        pl2.setImage('op2')
        fore:fade(1, 2)
        pl2.play('voice', '0002')
        pl2.wait(2)
    end
--]]
    return title()
end

function title()
    local r = rooms[randomItem(rooms)]

    back:fade(0, 0)
    fore:fade(0, 0)
    pl2.wait(0)

    room:setModels(r)
    room:setAnim(r.anim)
    room:setVisible(true)

    camera:setPath('A1cam1', true)

    fore:fade(1, 1)
    back:fade(1, 1)
    pl2.wait(1)

    local i = pl2.showMenu{ 'ストーリーモード', 'とことんＨモード', '終了' }
    print('selected',i)

    back:fade(0, 1)
    pl2.wait(1)
    
    return ({ storyA, title, pl2.quit })[i]()
end

function h_mode()
end

function storyA()
    print'storyA'

    pl2.play('voice', '0007')
    pl2.play('music', nil, 4)
    
    fore:fade(0, 2)
    pl2.wait(2)

    back:fade(0, 0)
    pl2.setTitle(false)
    pl2.setQuit(true)
    
    ani:setModels{ 'ani_bodyB_00' }
    ani:setPoint('loc_pos00')
    
    imo:setVisible(false)
    ani:setVisible(false)
    room:setVisible(false)
    
    room:setModels{ 'room_01' }
    camera:setPoint('loc_pos00')
    camera:setPath('A1cam1', true)

    fore:fade(1, 0)
    pl2.play('bgsound', 'se09')
    pl2.setWindow(true)
    back:fade(1, 1)
    pl2.wait(1)

    pl2.showText '‥‥‥‥‥勝った！'
    pl2.showText '俺は自由だ！！'
    pl2.showText '英語で言うとフリーダム。'
    pl2.showText 'まあー、何に勝ったかと言えば受験戦争だ。'
    pl2.showText 'つまりは大学に受かったのである。'
    pl2.showText '大学が始まって数ヶ月経った今も、俺は毎日この喜びを満喫している。'
    pl2.showText '一昨年までは勉強なんて、なーんもする気の無かった\n俺だが今年は一味違っていた。'
    pl2.showText '魔女っ娘が主人公の「たんもえ」と言うナイスな参考書が俺のやる気を刺激、嘘のように勉強がはかどったのだ。'
    pl2.showText 'いい世の中になった物である。'
    pl2.showText 'ありがとう「たんもえ」、ありがとうゆとり教育。'
    pl2.showText '日本の将来が心配です。'
    pl2.showText '‥‥‥‥‥さておき。'
    pl2.showText '自由も手に入れたことだし、これで心置きなく積みゲーになっていた「Ｈａｔｅ／ｓｔａｙ　ｔｉｇｈｔ」\n（初回版）が遊べるってもんだ。'
    pl2.showText '迫り来る６人の魔法使いに酷く嫌われながら‥‥\n（ここが　Ｈａｔｅ）'
    pl2.showText '命がけの試練に耐え抜き‥‥\n（ここが　ｓｔａｙ　ｔｉｇｈｔ）'
    pl2.showText '愛を勝ち取ると言う壮絶なエロゲーである。'
    pl2.showText '総プレイ時間は６０時間とも言われる大作ゲームだが\n今の俺は腐るほど時間がある。'
    pl2.showText 'コーラとカップ麺は三日分用意してある。\nもはや徹夜もじさない覚悟だ。'
    pl2.showText 'クリアするまでこの部屋を一歩も外に出ないぞっ\n後はぶっ倒れるまでやるだけだ！（何を？）'
    pl2.showText 'レッツゴー！！目眩く官能の世界へ！！'
    pl2.showText '誰にも俺の邪魔をさせないゼェ〜〜！！'
    
    pl2.play('sound', 'se25')
    
    pl2.showText 'っと、思った所で早速邪魔が入った。'
    
    pl2.play('voice', '0037')
    imo2 '「おにいちゃ〜ん、居るの〜？」'

    pl2.showText 'ドアの向こうから俺を呼ぶ声がする。'
    pl2.showText 'ちぃ〜こんな時に来なくてもいいだろう、\nタイミング悪すぎるぞあいつ。'
    pl2.showText 'ここは一つ‥‥‥‥‥\n大学に受かったこの俺の冴えた頭脳であいつをやり過ごすとするか。'

    pl2.play('voice', '0038')
    imo2 '「‥‥‥‥‥あれ？居ないの？」'
    
    ani '「‥‥‥‥‥‥‥‥‥‥‥」'
    ani '「‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥いないよ〜」'

    pl2.play('voice', '0039')
    imo2 '「あ、そっか‥‥‥‥じゃーまた後で来るね〜」'

    pl2.play('sound', 'se17')

    pl2.showText 'ドアの前から足音が遠ざかる。'
    pl2.showText '‥‥ふう、危ない。\nとっさの機転で俺のプライベートタイムが守られたようだ。'
    pl2.showText 'では気を取り直して、レッツゴー！！\n目眩く官能の‥‥‥'
    
    pl2.play('sound', 'se18')

    pl2.play('voice', '0040')
    imo2 '「‥‥‥って、居るじゃない！！」'
    
    ani '「ふふおっ！！」'

    pl2.showText '驚きのあまり不思議な言葉を発してしまった。'
    pl2.showText '完璧にやり過ごせたと思ったのだが、居ることを気づかれてしまったようだ。'
    pl2.showText 'やるな、流石は俺の妹だ。'
    
    pl2.play('voice', '0041')
    imo '「ねぇ〜おにいちゃん〜〜居るんでしょ〜〜？\n‥‥‥‥返事してよ〜〜」'

    pl2.play('bgsound', nil)
    back:fade(0, 1)
    camera:setLocked(true)

    pl2.showText('そう、この可愛い声の主は俺の妹「' .. imo:getName() .. '」だ。')

    pl2.play('music', nil, 2)

    pl2.showText '‥‥‥‥‥‥‥‥‥‥‥‥‥'
    pl2.showText '‥‥‥‥‥‥あれ？'
    pl2.showText 'あいつどんな容姿してたっけ？'
    pl2.showText 'アルツを疑いながらも、とりあえず妹を思い出して見る。'
    pl2.showText 'まずは髪型は茶色の長いツィンテールだ。'
    pl2.showText 'うん、それは間違いない。'
    pl2.showText 'それから‥‥‥‥'
    
    local face, eyes, body
    
    face = ({ 'A', 'B', 'C' })[pl2.showMenu{
        '普通に綺麗な顔つき', '意思の強そうな顔つき', '優しくて落ち着く顔つき' }]

    pl2.showText 'そして瞳の色は‥'

    
    while not eyes do
        eyes = ({ 0, 1, 2, 3, 4, 6 })[pl2.showMenu{
            '燃える赤色だ', '大自然の緑色だ', 'おいしそうなオレンジ色だ', '目立つ黄色だ',
            '深いコバルトブルーだ', '自然な茶色だ', 'いや、他の色だ‥' }] or
               ({ 7, 8, 9, 10, 11 })[pl2.showMenu{
            '馴染みのある黒だ', '怪しい紫色だ', 'カッコいい銀色だ', '珍しい黄金色だ',
            '綺麗な水色だ', 'いや、他の色だ‥' }]
    end

    pl2.showText '顔はそんな感じだったと思う。'
    pl2.showText 'ちなみに身体は‥‥'

    body = ({ 0, 1, 2, 3, 4 })[pl2.showMenu{
        '普通の肌色だ', '病弱な肌色だ', '健康的な小麦色だ', '魅惑のビキニ焼けだ',
        'マニア推奨のスク水焼けだ' }]

    imo:setModels{
        ('imo_body%s_%02d'):format(face, body),
        ('imo_eye_%02d'):format(eyes),
        [13] = 'imo_hairB_06' }
    imo:setPoint('loc_pos00')
    imo:setAnim('event_01')
    
    imo:setVisible(true)
    camera:setPath('camA', false)
    
    back:fade(1, 1)

    pl2.showText '‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥‥'
    
    back:fade(0, 1)
    pl2.wait(1)
    
    imo:setVisible(false)

    pl2.play('music', nil, 2)

    camera:setPath('A1cam1', true)
    imo:setAnim('event_06')
    imo:setAnim('event_02')
    
    pl2.play('bgsound', 'se09')
    back:fade(1, 1)
    camera:setLocked(false)
    pl2.wait(1)

    pl2.showText 'そうそう〜こんな感じだ。（何故か裸）'
    pl2.showText '‥‥‥‥で、そんな&3だが、おにいちゃんの俺が言うのもなんだがそこら辺にいる女の子よりも可愛いと思う。'
    pl2.showText 'ただ、いつも何かと理由を見つけては俺の部屋にやってくるのが困りものだ。'
    pl2.showText 'それはもう自分の部屋よりも俺の部屋で過ごす時間の方が多いんじゃないか？って思えるぐらい長く居座る。'
    pl2.showText '普段なら適当に相手するし、どこに居ようと別に構わないのだが‥‥‥‥'
    pl2.showText '俺はエロゲーを遊ばなくてはいけないと言う崇高な使命があり義務があり責任がある。'
    pl2.showText 'つまり、今は邪魔だ。'

    pl2.play('voice', '0042')
    imo '「もしも〜し？おにいちゃん居るんでしょ？\n‥‥‥‥入るよ〜？」'    

    ani '「あっ、おい、ちょっと待て！！」'
    
    back:fade(0, 0.2)
    
    pl2.play('sound', 'se07')

    pl2.showText '制止の声も空しく妹はドアを開けて部屋に入って来た。'
    
    pl2.play('bgsound', nil)

    local outfits = {
        {   [ 3] = 'imo_underA_01A',
            [ 4] = 'imo_underA_01B',
            [ 5] = 'imo_socksB_01',
	        [ 6] = 'imo_cosA_01A',
	        [ 7] = 'imo_cosK_03B',
	        [12] = 'imo_shoesB_03' },
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_05A',
	        [7] = 'imo_cosA_05B',
	        [13] = 'imo_shoesB_04' },
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_06A',
	        [7] = 'imo_cosK_03B',
	        [13] = 'imo_shoesB_04' },
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [6] = 'imo_cosA_06A',
	        [7] = 'imo_cosA_01B',
	        [13] = 'imo_shoesB_01', },
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_01A',
	        [7] = 'imo_cosA_04B',
	        [13] = 'imo_shoesB_04' },
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_00',
	        [6] = 'imo_cosK_06A',
	        [7] = 'imo_cosK_00B',
	        [12] = 'imo_shoesD_00',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksA_05',
	        [6] = 'imo_cosA_07A',
	        [7] = 'imo_cosA_07B',
	        [12] = 'imo_shoesB_01',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_03A',
	        [7] = 'imo_cosK_05B',
	        [12] = 'imo_shoesB_03',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksC_01',
	        [6] = 'imo_cosK_07A',
	        [7] = 'imo_cosA_03B',
	        [12] = 'imo_shoesB_03',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [6] = 'imo_cosK_05A',
	        [7] = 'imo_cosK_06B',
	        [12] = 'imo_shoesB_01',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_04A',
	        [7] = 'imo_cosA_04B',
	        [12] = 'imo_shoesD_05',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_00A',
	        [7] = 'imo_cosA_06B',
	        [12] = 'imo_shoesB_00',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_00A',
	        [7] = 'imo_cosK_07B',
	        [12] = 'imo_shoesB_00',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_05A',
	        [7] = 'imo_cosK_07B',
	        [12] = 'imo_shoesB_04',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [6] = 'imo_cosA_06A',
	        [7] = 'imo_cosK_06B',
	        [12] = 'imo_shoesB_01',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_04A',
	        [7] = 'imo_cosK_04B',
	        [12] = 'imo_shoesB_04',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosK_07A',
	        [7] = 'imo_cosK_01B',
	        [12] = 'imo_shoesB_04',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_01A',
	        [7] = 'imo_cosA_07B',
	        [12] = 'imo_shoesB_04',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [5] = 'imo_socksB_01',
	        [6] = 'imo_cosA_04A',
	        [7] = 'imo_cosK_04B',
	        [12] = 'imo_shoesB_04',	},
        {	[3] = 'imo_underA_01A',
	        [4] = 'imo_underA_01B',
	        [6] = 'imo_cosA_03A',
	        [7] = 'imo_cosA_03B',
	        [12] = 'imo_shoesD_00',	},
        }

    imo:setModels(randomItem(outfits), true)

    imo:setVisible(true)
    
    camera:setPath('B1cam1', false)
    
    pl2.play('music', 'BGM04')
    back:fade(1, 1)
    pl2.wait(1)

    imo '「‥‥‥‥‥‥‥‥‥‥‥‥‥」'
    
    imo:setAnim('event_06')
    camera:setPath('cam01', false)
    
    pl2.play('voice', '0043')
    imo '「‥‥‥‥‥‥‥‥‥‥‥あっ‥‥‥」'

    return storyB()
end

function storyB()
    print'storyB'

	room:setVisible(true)
	imo:setVisible(true)
    pl2.setWindow(true)

	room:setModels{ 'eventBG_01' }
	camera:setLocked(true)
	imo:setPoint('loc_pos00')
	ani:setPoint('loc_pos00')
	camera:setPoint('loc_pos00')
	camera:setPath('cam05', false)
	back:fade(1, 2)
	pl2.wait(2)

    pl2.play('voice', '0085')
    
    imo '「おにいちゃん‥‥‥‥ここって‥‥‥‥」'

	ani '「そう！！ここは乙女の園、レディスインナーウェア専門店「ボディースクラッチ」だ！！」'

	pl2.showText '最近はこういった下着の専門店が増えて来てる。'

	pl2.showText '流石の俺も一人では入れないこのお店も、今日は妹と言うカモフラージュがあるから大丈夫だ。'

    --camera:setPath('cam03', false)

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
