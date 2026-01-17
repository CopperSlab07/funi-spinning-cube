program funni_spinning_cube;
uses crt, sdl2, sysutils, math;

label skip;

type
    Vec2li = record
	x : LongInt;
	y : LongInt;
    end;

    Vec2f = record
	x : Real;
	y : Real;
    end;
    
    Vec3f = record
	x : Real;
	y : Real;
	z : Real;
    end;
    
    TCube = array [1..8,1..3] of Real;
    TCubeIndice = array [1..2,1..12] of Integer;

const
    win_x = 100;
    win_y = 100;
    win_width = 800;
    win_height = 800;
    win_flags = SDL_WINDOW_SHOWN {+ SDL_WINDOW_RESIZABLE};
    renderer_flags = SDL_RENDERER_ACCELERATED + SDL_RENDERER_PRESENTVSYNC;

    bg_r = 16;
    bg_g = 16;
    bg_b = 16;
    bg_a = 255;

    fg_r = 16;
    fg_g = 255;
    fg_b = 16;
    fg_a = 255;

    FPS = 60;
    sdl_dt = round(1000/60);
    delta_time = 1/60;
    pi_ap = 3.14;

var
    compiled : TSDL_version;
    linked : TSDL_version;
    win : PSDL_Window;
    renderer : PSDL_Renderer;
    event : TSDL_Event;
    state : ShortInt;
    {small_square : TSDL_Rect = (x:100; y:100; w:100; h:100);}
    dz : Real = 1.0;
    TestCube : TCube = (
	(0.25, 0.25, 0.25),
	(0.25, -0.25, 0.25),
	(-0.25, -0.25, 0.25),
	(-0.25, 0.25, 0.25),

	(0.25, 0.25, -0.25),
	(0.25, -0.25, -0.25),
	(-0.25, -0.25, -0.25),
	(-0.25, 0.25, -0.25)
    );
    angle : Real = 0;
    TestIndice : TCubeIndice = (
	(1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4),
	(2, 3, 4, 1, 6, 7, 8, 5, 5, 6, 7, 8)
    );

procedure panic;
begin
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 'Error Box',  SDL_GetError, nil);
    Exit;
end;

function rotate(position : Vec3f; angle : Real) : Vec3f;
var
    out : Vec3f;
    sinus : Real;
    cosinus : Real;
begin
    sincos(angle, sinus, cosinus);
    out.x := position.x * cosinus - position.z * sinus;
    out.y := position.y;
    out.z := position.x * sinus + position.z * cosinus;
    rotate := out;
end;

function transform_z(position : Vec3f; dz : Real) : Vec3f;
var
    out : Vec3f;
begin
    out.x := position.x;
    out.y := position.y;
    out.z := (position.z + dz);
    transform_z := out;
end;

function project(position : Vec3f) : Vec2f;
var
    out : Vec2f;
begin
    out.x := (position.x/position.z);
    out.y := (position.y/position.z);
    project := out;
end;

function convert(position : Vec2f) : Vec2li;
var
    out : Vec2li;
begin
    out.x := round(((position.x + 1)/2) * win_width);
    out.y := round((1 - (position.y + 1)/2) * win_height);
    convert := out;
end;

procedure DrawPoint(position: Vec2li; size: LongInt);
var
    Placeholder : TSDL_Rect;
begin
    Placeholder.x := (position.x - size);
    Placeholder.y := (position.y - size);
    Placeholder.w := (size*2);
    Placeholder.h := (size*2);
    SDL_SetRenderDrawColor(renderer, fg_r, fg_g, fg_b, fg_a);
    SDL_RenderDrawRect(renderer, @Placeholder);
end;

procedure DrawLine(position_1: Vec2li; position_2: Vec2li);
begin
    SDL_SetRenderDrawColor(renderer, fg_r, fg_g, fg_b, fg_a);
    SDL_RenderDrawLine(renderer, position_1.x, position_1.y, position_2.x, position_2.y);
end;

procedure cube_lines(Cube: TCube; Indice: TCubeIndice);
var
    index : Integer;
    temp_point_1 : Vec3f;
    temp_point_2 : Vec3f;
begin
    for index := 1 to 12 do
	begin
	    temp_point_1.x := Cube[Indice[1][index]][1];
	    temp_point_1.y := Cube[Indice[1][index]][2];
	    temp_point_1.z := Cube[Indice[1][index]][3];

	    temp_point_2.x := Cube[Indice[2][index]][1];
	    temp_point_2.y := Cube[Indice[2][index]][2];
	    temp_point_2.z := Cube[Indice[2][index]][3];
	    DrawLine(convert(project(transform_z(rotate(temp_point_1, angle), dz))), convert(project(transform_z(rotate(temp_point_2, angle), dz))));
	end;
end;

procedure cube_points(Cube: TCube);
var
    index : Integer;
    temp_point : Vec3f;
begin
    for index := 1 to 8 do
	begin
	    temp_point.x := Cube[index][1];
	    temp_point.y := Cube[index][2];
	    temp_point.z := Cube[index][3];
	    DrawPoint(convert(project(transform_z(rotate(temp_point, angle), dz))), 10);
	end;
end;

begin
    SDL_VERSION(compiled);
    SDL_GetVersion(@linked);
    WriteLn('compiled against SDL version: ' + IntToStr(compiled.major), '.', IntToStr(compiled.minor) , '.', IntToStr(compiled.patch));
    WriteLn('linked against SDL version: ' + IntToStr(linked.major), '.', IntToStr(linked.minor), '.', IntToStr(linked.patch)); 
    
    if SDL_Init(SDL_INIT_VIDEO) < 0 then panic;

    win := SDL_CreateWindow('Hello nig-', win_x, win_y, win_width, win_height, win_flags);
    if win = nil then panic;

    renderer := SDL_CreateRenderer(win, -1, renderer_flags);
    if renderer = nil then panic;
    
    SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    state := 0;
    repeat
    begin
	if event.type_ = SDL_QUITEV then state := -1;
	if SDL_PollEvent(@event) = 0 then goto skip;
	case event.key.keysym.sym of
	    SDLK_ESCAPE : state := -1;
	    {SDLK_a	: small_square.x-=20;
	    SDLK_d	: small_square.x+=20;
	    SDLK_w	: small_square.y-=20;
	    SDLK_s	: small_square.y+=20;}
	    {else WriteLn('Pressed an unconfigured button: ', event.key.keysym.sym);}
	end;
skip:
	angle += delta_time;
	if angle > (2 * pi_ap) then angle -= (2 * pi_ap);
	SDL_SetRenderDrawColor(renderer, bg_r, bg_g, bg_b, bg_a);
	SDL_RenderClear(renderer);
	{SDL_SetRenderDrawColor(renderer, fg_r, fg_g, fg_b, fg_a);}
	{DrawPoint(convert(project(transform_z(rotate(TestPoint, angle), dz))), 10);}
	{cube_points(TestCube);}
	cube_lines(TestCube, TestIndice);
	SDL_RenderPresent(renderer);
	SDL_Delay(sdl_dt);
    end;
    until state < 0;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit;
end.
