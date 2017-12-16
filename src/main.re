type browser;

type page;

type response;

type buffer;

[@bs.module "puppeteer"] external launch : 'a => Js.Promise.t(browser) = "launch";

[@bs.send] external newPage : browser => Js.Promise.t(page) = "newPage";

[@bs.send] external goto : (page, ~url: string) => Js.Promise.t(response) = "goto";

[@bs.send] external screenshot : (page, {. "path": string}) => Js.Promise.t(buffer) = "screenshot";

[@bs.send] external close : browser => Js.Promise.t(unit) = "close";

let url = "https://app.grammarly.com/";

let signIn = "https://www.grammarly.com/signin";

let _ = {
  open Js.Promise;
  let _ =
    launch()
    |> then_(
         (browser) =>
           browser
           |> newPage
           |> then_(
                (page) =>
                  page
                  |> goto(~url=signIn)
                  |> then_((_res) => screenshot(page, {"path": "screenshot/example.png"}))
                  |> then_((_) => close(browser))
              )
       );
  ()
};
