type browser;

type page;

type response;

type buffer;

type element;

[@bs.module "puppeteer"]
external launch : option({. "headless": Js.boolean}) => Js.Promise.t(browser) =
  "launch";

[@bs.send] external newPage : browser => Js.Promise.t(page) = "newPage";

[@bs.send] external goto : (page, ~url: string) => Js.Promise.t(response) = "goto";

[@bs.send] external screenshot : (page, {. "path": string}) => Js.Promise.t(buffer) = "screenshot";

[@bs.send] external type_ : (page, string, string, option({. delay: int})) => Js.Promise.t(unit) =
  "type";

[@bs.send]
external click :
  (page, string, option({. button: string, clickCount: int, delay: int})) => Js.Promise.t(element) =
  "click";

[@bs.send] external waitForFunction : (page, string) => Js.Promise.t(unit) = "waitForFunction";

[@bs.send] external waitForSelector : (page, string) => Js.Promise.t(unit) = "waitForSelector";

[@bs.send] external close : browser => Js.Promise.t(unit) = "close";

let url = "https://app.grammarly.com/";

let signIn = "https://www.grammarly.com/signin";

let _ = {
  open Js.Promise;
  let (email, password) =
    try (Sys.getenv("GRMLY_EMAIL"), Sys.getenv("GRMLY_PASSWORD")) {
    | Not_found =>
      Printf.printf("Environment variables GRMLY_EMAIL and GRMLY_PASSWORD are mandatory");
      raise(Not_found)
    };
  let _ =
    launch(Some({"headless": Js.false_}))
    |> then_(
         (browser) =>
           browser
           |> newPage
           |> then_(
                (page) => {
                  Js.log("OK");
                  page
                  |> goto(~url=signIn)
                  |> then_((_res) => screenshot(page, {"path": "screenshot/login.png"}))
                  |> then_((_) => type_(page, "input[type=email]", email, None))
                  |> then_((_) => type_(page, "input[type=password]", password, None))
                  |> then_((_res) => screenshot(page, {"path": "screenshot/login-input.png"}))
                  |> then_((_) => click(page, "button[type=submit]", None))
                  |> then_((_) => waitForSelector(page, "div[data-name=new-doc-item]"))
                  |> then_((_res) => screenshot(page, {"path": "screenshot/app.png"}))
                  |> then_((_) => close(browser))
                }
              )
       );
  ()
};
